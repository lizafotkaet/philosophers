#!/usr/bin/env python3
"""
Comprehensive test suite for the Philosophers project.
Tests various timing combinations and validates output correctness.

Usage:
    python3 verify.py [--verbose] [--philo-path ./philo]
"""

import subprocess
import sys
import re
import time
import argparse
from dataclasses import dataclass
from pathlib import Path
from typing import Optional
from enum import Enum


# ============================================================================
# Data Structures
# ============================================================================

class Action(Enum):
    FORK = "has taken a fork"
    PUT_FORK = "has put down a fork"
    EATING = "is eating"
    SLEEPING = "is sleeping"
    THINKING = "is thinking"
    DIED = "died"


@dataclass
class LogEntry:
    timestamp: int  # milliseconds
    philo_id: int
    action: Action
    raw_line: str


@dataclass
class TestCase:
    name: str
    num_philos: int
    time_to_die: int
    time_to_eat: int
    time_to_sleep: int
    num_meals: Optional[int] = None
    expect_death: bool = False
    max_runtime_ms: int = 10000  # 10 seconds default
    description: str = ""


@dataclass
class TestResult:
    test_case: TestCase
    passed: bool
    errors: list[str]
    warnings: list[str]
    runtime_ms: int
    death_detected: bool
    death_time_ms: Optional[int]
    output_lines: int
    raw_output: str = ""


# ============================================================================
# Output Parser
# ============================================================================

def parse_line(line: str) -> Optional[LogEntry]:
    """Parse a single line of philosopher output."""
    line = line.strip()
    if not line:
        return None

    # Expected format: <timestamp> <philo_id> <action>
    patterns = [
        (r"^(\d+)\s+(\d+)\s+has taken a fork$", Action.FORK),
        (r"^(\d+)\s+(\d+)\s+has put down a fork$", Action.PUT_FORK),
        (r"^(\d+)\s+(\d+)\s+is eating$", Action.EATING),
        (r"^(\d+)\s+(\d+)\s+is sleeping$", Action.SLEEPING),
        (r"^(\d+)\s+(\d+)\s+is thinking$", Action.THINKING),
        (r"^(\d+)\s+(\d+)\s+died$", Action.DIED),
    ]

    for pattern, action in patterns:
        match = re.match(pattern, line)
        if match:
            return LogEntry(
                timestamp=int(match.group(1)),
                philo_id=int(match.group(2)),
                action=action,
                raw_line=line
            )

    return None


def parse_output(output: str) -> list[LogEntry]:
    """Parse the entire output into log entries."""
    entries = []
    for line in output.strip().split('\n'):
        entry = parse_line(line)
        if entry:
            entries.append(entry)
    return entries


# ============================================================================
# Validators
# ============================================================================

def validate_timestamps_ascending(entries: list[LogEntry]) -> list[str]:
    """Check that timestamps are non-decreasing."""
    errors = []
    for i in range(1, len(entries)):
        if entries[i].timestamp < entries[i-1].timestamp:
            errors.append(
                f"Timestamp went backwards: {entries[i-1].timestamp} -> {entries[i].timestamp} "
                f"at line {i+1}"
            )
    return errors


def validate_no_output_after_death(entries: list[LogEntry]) -> list[str]:
    """Check that no messages appear after a death (within 10ms tolerance)."""
    errors = []
    death_entry = None

    for entry in entries:
        if entry.action == Action.DIED:
            death_entry = entry
            break

    if death_entry:
        for entry in entries:
            if entry.timestamp > death_entry.timestamp:
                errors.append(
                    f"Message after death: '{entry.raw_line}' "
                    f"(death at {death_entry.timestamp}, message at {entry.timestamp})"
                )

    return errors


def validate_death_timing(entries: list[LogEntry], time_to_die: int) -> list[str]:
    """
    Check that death occurs within 10ms of expected time.
    A philosopher should die if they haven't eaten within time_to_die ms.
    """
    errors = []

    # Track last meal time for each philosopher (start time = 0)
    last_meal: dict[int, int] = {}
    first_timestamp = entries[0].timestamp if entries else 0

    death_entry = None
    for entry in entries:
        if entry.action == Action.DIED:
            death_entry = entry
            break

        if entry.action == Action.EATING:
            last_meal[entry.philo_id] = entry.timestamp

    if death_entry:
        philo_id = death_entry.philo_id
        last_eaten = last_meal.get(philo_id, first_timestamp)
        time_since_meal = death_entry.timestamp - last_eaten

        # Death should occur at approximately time_to_die after last meal
        # Allow 10ms tolerance
        if time_since_meal < time_to_die:
            errors.append(
                f"Philosopher {philo_id} died too early: "
                f"{time_since_meal}ms since last meal, time_to_die={time_to_die}ms"
            )
        elif time_since_meal > time_to_die + 10:
            errors.append(
                f"Philosopher {philo_id} death detected too late: "
                f"{time_since_meal}ms since last meal (should be ~{time_to_die}ms, tolerance 10ms)"
            )

    return errors


def validate_fork_usage(entries: list[LogEntry], num_philos: int) -> list[str]:
    """Check that philosophers take exactly 2 forks before eating."""
    errors = []
    fork_count: dict[int, int] = {i: 0 for i in range(1, num_philos + 1)}

    for entry in entries:
        if entry.action == Action.FORK:
            fork_count[entry.philo_id] += 1
        elif entry.action == Action.EATING:
            if fork_count[entry.philo_id] != 2:
                errors.append(
                    f"Philosopher {entry.philo_id} started eating with "
                    f"{fork_count[entry.philo_id]} forks at {entry.timestamp}ms"
                )
            fork_count[entry.philo_id] = 0  # Reset after eating

    return errors


def validate_philosopher_ids(entries: list[LogEntry], num_philos: int) -> list[str]:
    """Check that all philosopher IDs are valid."""
    errors = []
    for entry in entries:
        if entry.philo_id < 1 or entry.philo_id > num_philos:
            errors.append(
                f"Invalid philosopher ID {entry.philo_id} (expected 1-{num_philos})"
            )
    return errors


def validate_meal_count(entries: list[LogEntry], num_philos: int, num_meals: int) -> list[str]:
    """Check that all philosophers ate at least num_meals times."""
    errors = []
    meal_count: dict[int, int] = {i: 0 for i in range(1, num_philos + 1)}

    for entry in entries:
        if entry.action == Action.EATING:
            meal_count[entry.philo_id] += 1

    for philo_id, count in meal_count.items():
        if count != num_meals:
            errors.append(
                f"Philosopher {philo_id} only ate {count} times (expected {num_meals})"
            )

    return errors


def validate_single_death(entries: list[LogEntry]) -> list[str]:
    """Check that at most one death message appears."""
    errors = []
    death_count = sum(1 for e in entries if e.action == Action.DIED)

    if death_count > 1:
        errors.append(f"Multiple death messages: {death_count} deaths reported")

    return errors


def validate_state_transitions(entries: list[LogEntry], num_philos: int) -> list[str]:
    """
    Check that each philosopher follows valid state transitions:
    thinking -> fork -> fork -> eating -> sleeping -> thinking -> ...

    Valid transitions:
    - Initial state can be thinking or fork
    - thinking -> fork
    - fork -> fork (second fork)
    - fork -> eating (after 2 forks)
    - eating -> sleeping
    - sleeping -> thinking
    """
    errors = []

    # Track state for each philosopher
    # States: None (initial), 'thinking', 'fork1', 'fork2', 'eating', 'sleeping'
    state: dict[int, str] = {i: None for i in range(1, num_philos + 1)}
    fork_count: dict[int, int] = {i: 0 for i in range(1, num_philos + 1)}

    for entry in entries:
        if entry.action == Action.DIED:
            break

        philo = entry.philo_id
        current_state = state[philo]

        if entry.action == Action.THINKING:
            # Valid from: initial, sleeping
            if current_state not in (None, 'sleeping'):
                errors.append(
                    f"Philosopher {philo} invalid transition: {current_state} -> thinking "
                    f"at {entry.timestamp}ms"
                )
            state[philo] = 'thinking'
            fork_count[philo] = 0

        elif entry.action == Action.FORK:
            # Valid from: thinking, fork1 (taking second fork)
            if current_state == 'thinking' or current_state is None:
                state[philo] = 'fork1'
                fork_count[philo] = 1
            elif current_state == 'fork1':
                state[philo] = 'fork2'
                fork_count[philo] = 2
            else:
                errors.append(
                    f"Philosopher {philo} invalid transition: {current_state} -> fork "
                    f"at {entry.timestamp}ms"
                )

        elif entry.action == Action.EATING:
            # Valid from: fork2 (must have 2 forks)
            if current_state != 'fork2' or fork_count[philo] != 2:
                errors.append(
                    f"Philosopher {philo} started eating without 2 forks "
                    f"(state={current_state}, forks={fork_count[philo]}) at {entry.timestamp}ms"
                )
            state[philo] = 'eating'

        elif entry.action == Action.SLEEPING:
            # Valid from: eating
            if current_state != 'eating':
                errors.append(
                    f"Philosopher {philo} invalid transition: {current_state} -> sleeping "
                    f"at {entry.timestamp}ms"
                )
            state[philo] = 'sleeping'
            fork_count[philo] = 0

    return errors


def validate_no_adjacent_eating(entries: list[LogEntry], num_philos: int) -> list[str]:
    """
    Check that two neighboring philosophers are never eating at the same time.
    A neighbor cannot hold 2 forks (be eating) while I am eating.
    Philosophers i and i+1 are neighbors (wrapping around).
    """
    errors = []

    # Track how many forks each philosopher has
    fork_count: dict[int, int] = {i: 0 for i in range(1, num_philos + 1)}

    def left_neighbor(philo: int) -> int:
        return (philo - 2) % num_philos + 1

    def right_neighbor(philo: int) -> int:
        return philo % num_philos + 1

    for entry in entries:
        if entry.action == Action.DIED:
            break

        philo = entry.philo_id

        if entry.action == Action.FORK:
            fork_count[philo] += 1

            # After taking a fork, check if I have 2 forks (about to eat)
            # and a neighbor also has 2 forks (is eating)
            if fork_count[philo] == 2:
                ln = left_neighbor(philo)
                rn = right_neighbor(philo)

                if fork_count[ln] == 2:
                    errors.append(
                        f"Philosopher {philo} has 2 forks while left neighbor {ln} also has 2 forks "
                        f"at {entry.timestamp}ms"
                    )
                if fork_count[rn] == 2:
                    errors.append(
                        f"Philosopher {philo} has 2 forks while right neighbor {rn} also has 2 forks "
                        f"at {entry.timestamp}ms"
                    )

        elif entry.action == Action.EATING:
            # When eating starts, verify no neighbor has 2 forks
            ln = left_neighbor(philo)
            rn = right_neighbor(philo)

            if fork_count[ln] == 2:
                errors.append(
                    f"Philosopher {philo} started eating while left neighbor {ln} has 2 forks "
                    f"at {entry.timestamp}ms"
                )
            if fork_count[rn] == 2:
                errors.append(
                    f"Philosopher {philo} started eating while right neighbor {rn} has 2 forks "
                    f"at {entry.timestamp}ms"
                )

        elif entry.action == Action.PUT_FORK:
            # Philosopher releases one fork
            if fork_count[philo] > 0:
                fork_count[philo] -= 1

    return errors


def validate_hunger_timing(entries: list[LogEntry], num_philos: int, time_to_die: int) -> list[str]:
    """
    Check that the time between sleeping (end of eating) and next eating
    does not exceed time_to_die.

    A philosopher can only starve when NOT eating. The hunger timer runs:
    - From simulation start until first eating
    - From sleeping (finished eating) until next eating

    If sleeping->eating interval > time_to_die, philosopher should have died.
    """
    errors = []

    # Track last sleeping timestamp for each philosopher (= when they finished eating)
    # Initialize to simulation start (philosophers start "hungry")
    first_timestamp = entries[0].timestamp if entries else 0
    last_finished_eating: dict[int, int] = {i: first_timestamp for i in range(1, num_philos + 1)}

    for entry in entries:
        if entry.action == Action.DIED:
            # Stop checking after death
            break

        if entry.action == Action.SLEEPING:
            # Philosopher finished eating, hunger timer starts
            last_finished_eating[entry.philo_id] = entry.timestamp

        elif entry.action == Action.EATING:
            philo_id = entry.philo_id
            current_time = entry.timestamp

            # Check time since last finished eating (or simulation start)
            time_hungry = current_time - last_finished_eating[philo_id]

            if time_hungry > time_to_die:
                errors.append(
                    f"Philosopher {philo_id} was hungry for {time_hungry}ms "
                    f"(last finished eating at {last_finished_eating[philo_id]}ms, "
                    f"started eating at {current_time}ms, time_to_die={time_to_die}ms)"
                )

    return errors


# ============================================================================
# Test Runner
# ============================================================================

def run_test(test: TestCase, philo_path: str, verbose: bool = False) -> TestResult:
    """Run a single test case and return the result."""
    errors = []
    warnings = []

    # Build command
    cmd = [philo_path,
           str(test.num_philos),
           str(test.time_to_die),
           str(test.time_to_eat),
           str(test.time_to_sleep)]

    if test.num_meals is not None:
        cmd.append(str(test.num_meals))

    if verbose:
        print(f"  Running: {' '.join(cmd)}")

    # Run process with timeout
    start_time = time.time()
    try:
        timeout_sec = test.max_runtime_ms / 1000.0 + 1  # Add 1 second buffer
        result = subprocess.run(
            cmd,
            capture_output=True,
            text=True,
            timeout=timeout_sec
        )
        output = result.stdout
        stderr = result.stderr
    except subprocess.TimeoutExpired:
        errors.append(f"Test timed out after {timeout_sec}s")
        return TestResult(
            test_case=test,
            passed=False,
            errors=errors,
            warnings=warnings,
            runtime_ms=int(timeout_sec * 1000),
            death_detected=False,
            death_time_ms=None,
            output_lines=0,
            raw_output=""
        )

    runtime_ms = int((time.time() - start_time) * 1000)

    # Check for stderr output (warnings)
    if stderr.strip():
        warnings.append(f"stderr output: {stderr.strip()[:200]}")

    # Parse output
    entries = parse_output(output)

    if not entries:
        if test.num_philos == 1 and test.expect_death:
            # Single philosopher should die, but might not have output
            pass
        else:
            errors.append("No valid output lines parsed")
            return TestResult(
                test_case=test,
                passed=False,
                errors=errors,
                warnings=warnings,
                runtime_ms=runtime_ms,
                death_detected=False,
                death_time_ms=None,
                output_lines=0,
                raw_output=output
            )

    # Detect death
    death_entry = next((e for e in entries if e.action == Action.DIED), None)
    death_detected = death_entry is not None
    death_time_ms = death_entry.timestamp if death_entry else None

    # Run validators
    errors.extend(validate_timestamps_ascending(entries))
    errors.extend(validate_philosopher_ids(entries, test.num_philos))
    errors.extend(validate_single_death(entries))
    errors.extend(validate_fork_usage(entries, test.num_philos))
    errors.extend(validate_state_transitions(entries, test.num_philos))
    errors.extend(validate_no_adjacent_eating(entries, test.num_philos))

    # Check hunger timing (sleeping->eating interval should not exceed time_to_die)
    if not test.expect_death:
        errors.extend(validate_hunger_timing(entries, test.num_philos, test.time_to_die))

    if death_detected:
        errors.extend(validate_no_output_after_death(entries))
        errors.extend(validate_death_timing(entries, test.time_to_die))

    # Check death expectation
    if test.expect_death and not death_detected:
        # For tests expecting death, check if it ran too long without dying
        if runtime_ms > test.time_to_die + 500:
            errors.append(
                f"Expected death but none occurred after {runtime_ms}ms "
                f"(time_to_die={test.time_to_die}ms)"
            )

    # Check for unexpected death
    if not test.expect_death and death_detected:
        errors.append(
            f"Unexpected death at {death_time_ms}ms "
            f"(philosopher {death_entry.philo_id})"
        )

    # Check meal count for finite meal tests
    if test.num_meals is not None and not death_detected:
        errors.extend(validate_meal_count(entries, test.num_philos, test.num_meals))

    passed = len(errors) == 0

    return TestResult(
        test_case=test,
        passed=passed,
        errors=errors,
        warnings=warnings,
        runtime_ms=runtime_ms,
        death_detected=death_detected,
        death_time_ms=death_time_ms,
        output_lines=len(entries),
        raw_output=output
    )


# ============================================================================
# Test Cases Generator
# ============================================================================

def generate_test_cases() -> list[TestCase]:
    """Generate comprehensive test cases covering various timing scenarios."""
    tests = []

    # ========================================================================
    # Category 1: Single Philosopher (edge case)
    # ========================================================================
    tests.append(TestCase(
        name="single_philo_dies",
        num_philos=1,
        time_to_die=800,
        time_to_eat=200,
        time_to_sleep=200,
        expect_death=True,
        max_runtime_ms=2000,
        description="Single philosopher cannot eat (only one fork), should die"
    ))

    # ========================================================================
    # Category 2: No one should die (time_to_die > eat + sleep cycle)
    # ========================================================================
    tests.append(TestCase(
        name="no_death_2_philos",
        num_philos=2,
        time_to_die=800,
        time_to_eat=200,
        time_to_sleep=200,
        num_meals=5,
        expect_death=False,
        max_runtime_ms=5000,
        description="2 philosophers, ample time, 5 meals each"
    ))

    tests.append(TestCase(
        name="no_death_5_philos",
        num_philos=5,
        time_to_die=800,
        time_to_eat=200,
        time_to_sleep=200,
        num_meals=7,
        expect_death=False,
        max_runtime_ms=8000,
        description="Classic 5 philosophers, should survive"
    ))

    tests.append(TestCase(
        name="no_death_4_philos",
        num_philos=4,
        time_to_die=410,
        time_to_eat=200,
        time_to_sleep=200,
        num_meals=5,
        expect_death=False,
        max_runtime_ms=6000,
        description="4 philosophers with tight but survivable timing"
    ))

    tests.append(TestCase(
        name="no_death_5_philos_tight",
        num_philos=5,
        time_to_die=610,
        time_to_eat=200,
        time_to_sleep=200,
        num_meals=5,
        expect_death=False,
        max_runtime_ms=6000,
        description="5 philosophers with tight timing"
    ))

    # ========================================================================
    # Category 3: Death expected (time_to_die < eat + sleep or tight timing)
    # ========================================================================
    tests.append(TestCase(
        name="death_time_to_die_lt_eat",
        num_philos=4,
        time_to_die=100,
        time_to_eat=200,
        time_to_sleep=100,
        expect_death=True,
        max_runtime_ms=2000,
        description="time_to_die < time_to_eat, death inevitable"
    ))

    tests.append(TestCase(
        name="death_time_to_die_lt_sleep",
        num_philos=4,
        time_to_die=100,
        time_to_eat=50,
        time_to_sleep=200,
        expect_death=True,
        max_runtime_ms=2000,
        description="time_to_die < time_to_sleep, death inevitable"
    ))

    tests.append(TestCase(
        name="death_very_short_time_to_die",
        num_philos=5,
        time_to_die=60,
        time_to_eat=60,
        time_to_sleep=60,
        expect_death=True,
        max_runtime_ms=1000,
        description="Very short time_to_die, almost certain death"
    ))

    # ========================================================================
    # Category 4: Boundary cases for timing
    # ========================================================================
    tests.append(TestCase(
        name="boundary_exact_cycle",
        num_philos=2,
        time_to_die=400,
        time_to_eat=200,
        time_to_sleep=200,
        expect_death=True,
        max_runtime_ms=30000,
        description="time_to_die == eat + sleep, must eventually die due to scheduling delays"
    ))

    tests.append(TestCase(
        name="boundary_just_above_cycle",
        num_philos=2,
        time_to_die=420,
        time_to_eat=200,
        time_to_sleep=200,
        num_meals=3,
        expect_death=False,
        max_runtime_ms=3000,
        description="time_to_die slightly above eat + sleep"
    ))

    # ========================================================================
    # Category 5: Large number of philosophers
    # ========================================================================
    tests.append(TestCase(
        name="many_philos_50",
        num_philos=50,
        time_to_die=800,
        time_to_eat=200,
        time_to_sleep=200,
        num_meals=3,
        expect_death=False,
        max_runtime_ms=10000,
        description="50 philosophers, should handle concurrency"
    ))

    tests.append(TestCase(
        name="many_philos_100",
        num_philos=100,
        time_to_die=800,
        time_to_eat=200,
        time_to_sleep=200,
        num_meals=2,
        expect_death=False,
        max_runtime_ms=10000,
        description="100 philosophers stress test"
    ))

    tests.append(TestCase(
        name="many_philos_200",
        num_philos=200,
        time_to_die=1000,
        time_to_eat=200,
        time_to_sleep=200,
        num_meals=2,
        expect_death=False,
        max_runtime_ms=10000,
        description="200 philosophers maximum stress test"
    ))

    # ========================================================================
    # Category 6: Finite meals edge cases
    # ========================================================================
    tests.append(TestCase(
        name="one_meal_each",
        num_philos=5,
        time_to_die=800,
        time_to_eat=200,
        time_to_sleep=200,
        num_meals=1,
        expect_death=False,
        max_runtime_ms=3000,
        description="Each philosopher eats exactly once"
    ))

    tests.append(TestCase(
        name="many_meals",
        num_philos=3,
        time_to_die=800,
        time_to_eat=100,
        time_to_sleep=100,
        num_meals=10,
        expect_death=False,
        max_runtime_ms=8000,
        description="Many meals per philosopher (3 philos)"
    ))

    # ========================================================================
    # Category 7: Asymmetric timing
    # ========================================================================
    tests.append(TestCase(
        name="long_eat_short_sleep",
        num_philos=5,
        time_to_die=800,
        time_to_eat=400,
        time_to_sleep=100,
        expect_death=True,
        max_runtime_ms=3000,
        description="Long eating time - with 5 philos, only 2 can eat at once, others wait 2+ cycles (800ms+) and die"
    ))

    tests.append(TestCase(
        name="short_eat_long_sleep",
        num_philos=5,
        time_to_die=800,
        time_to_eat=100,
        time_to_sleep=400,
        num_meals=3,
        expect_death=False,
        max_runtime_ms=8000,
        description="Short eating time, long sleep (5 philos)"
    ))

    # ========================================================================
    # Category 8: Minimum values
    # ========================================================================
    tests.append(TestCase(
        name="minimum_times",
        num_philos=3,
        time_to_die=200,
        time_to_eat=60,
        time_to_sleep=60,
        num_meals=5,
        expect_death=False,
        max_runtime_ms=4000,
        description="Minimum reasonable timing values (3 philos)"
    ))

    # ========================================================================
    # Category 9: Even vs odd philosophers
    # ========================================================================
    tests.append(TestCase(
        name="even_philos_6",
        num_philos=6,
        time_to_die=600,
        time_to_eat=200,
        time_to_sleep=200,
        num_meals=4,
        expect_death=False,
        max_runtime_ms=6000,
        description="Even number of philosophers (6)"
    ))

    tests.append(TestCase(
        name="odd_philos_7",
        num_philos=7,
        time_to_die=600,
        time_to_eat=200,
        time_to_sleep=200,
        num_meals=4,
        expect_death=False,
        max_runtime_ms=6000,
        description="Odd number of philosophers (7)"
    ))

    tests.append(TestCase(
        name="odd_philos_3",
        num_philos=3,
        time_to_die=610,
        time_to_eat=200,
        time_to_sleep=200,
        num_meals=5,
        expect_death=False,
        max_runtime_ms=6000,
        description="3 philosophers"
    ))

    # ========================================================================
    # Category 10: Death timing precision (10ms rule)
    # ========================================================================
    tests.append(TestCase(
        name="death_timing_precision",
        num_philos=4,
        time_to_die=310,
        time_to_eat=200,
        time_to_sleep=100,
        expect_death=True,
        max_runtime_ms=2000,
        description="Test death detection within 10ms"
    ))

    return tests


# ============================================================================
# Main Entry Point
# ============================================================================

def print_result(result: TestResult, verbose: bool = False):
    """Print a test result."""
    status = "✅ PASS" if result.passed else "❌ FAIL"
    print(f"  {status} - {result.test_case.name}")
    print(f"    {result.test_case.description}")
    print(f"    Runtime: {result.runtime_ms}ms, Lines: {result.output_lines}", end="")
    if result.death_detected:
        print(f", Death at: {result.death_time_ms}ms")
    else:
        print()

    if result.errors:
        for error in result.errors[:5]:  # Limit error output
            print(f"    ⚠️  {error}")
        if len(result.errors) > 5:
            print(f"    ... and {len(result.errors) - 5} more errors")

    if verbose and result.warnings:
        for warning in result.warnings:
            print(f"    ℹ️  {warning}")


def save_failed_log(result: TestResult, log_dir: str = "failed_logs"):
    """Save the raw output of a failed test to a log file."""
    import os
    os.makedirs(log_dir, exist_ok=True)

    test = result.test_case
    filename = f"{test.name}.log"
    filepath = os.path.join(log_dir, filename)

    with open(filepath, 'w') as f:
        f.write(f"# Test: {test.name}\n")
        f.write(f"# Description: {test.description}\n")
        f.write(f"# Parameters: {test.num_philos} {test.time_to_die} {test.time_to_eat} {test.time_to_sleep}")
        if test.num_meals is not None:
            f.write(f" {test.num_meals}")
        f.write("\n")
        f.write(f"# Runtime: {result.runtime_ms}ms\n")
        f.write(f"# Death detected: {result.death_detected}")
        if result.death_time_ms:
            f.write(f" at {result.death_time_ms}ms")
        f.write("\n")
        f.write(f"# Errors:\n")
        for error in result.errors:
            f.write(f"#   - {error}\n")
        f.write(f"#\n# Raw output:\n")
        f.write(result.raw_output)

    print(f"    📄 Log saved to: {filepath}")


def main():
    parser = argparse.ArgumentParser(description="Philosophers test suite")
    parser.add_argument("--verbose", "-v", action="store_true", help="Verbose output")
    parser.add_argument("--philo-path", default="./philo", help="Path to philo executable")
    parser.add_argument("--test", "-t", help="Run specific test by name")
    parser.add_argument("--list", "-l", action="store_true", help="List all tests")
    parser.add_argument("--repeat", "-r", type=int, default=1, help="Number of times to run the test suite (default: 1)")
    args = parser.parse_args()

    tests = generate_test_cases()

    if args.list:
        print("Available tests:")
        for test in tests:
            if test.expect_death:
                death_str = "expects death"
            else:
                death_str = "no death"
            meals_str = f", {test.num_meals} meals" if test.num_meals else ""
            print(f"  {test.name}: {test.num_philos}p, die={test.time_to_die}, "
                  f"eat={test.time_to_eat}, sleep={test.time_to_sleep}{meals_str} ({death_str})")
        return

    if args.test:
        tests = [t for t in tests if t.name == args.test]
        if not tests:
            print(f"❌ Test not found: {args.test}")
            sys.exit(1)

    total_passed = 0
    total_failed = 0
    num_runs = args.repeat

    for run in range(1, num_runs + 1):
        if num_runs > 1:
            print(f"{'=' * 60}")
            print(f"🔄 Run {run}/{num_runs}")
            print(f"{'=' * 60}\n")

        print(f"🧪 Running {len(tests)} tests...\n")

        passed = 0
        failed = 0

        for test in tests:
            result = run_test(test, './philo', args.verbose)
            print_result(result, args.verbose)

            if result.passed:
                passed += 1
            else:
                failed += 1
                save_failed_log(result, log_dir=f"failed_logs/run_{run}" if num_runs > 1 else "failed_logs")

            print()

        print("=" * 60)
        print(f"Run {run} Results: {passed} passed, {failed} failed out of {len(tests)} tests")

        total_passed += passed
        total_failed += failed

    if num_runs > 1:
        print(f"\n{'=' * 60}")
        print(f"📊 Total Results ({num_runs} runs):")
        print(f"   Passed: {total_passed}/{len(tests) * num_runs}")
        print(f"   Failed: {total_failed}/{len(tests) * num_runs}")
        print(f"{'=' * 60}")

    if total_failed > 0:
        sys.exit(1)


if __name__ == "__main__":
    main()
