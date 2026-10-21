#!/usr/bin/env python3
"""
LumiRover Telemetry & Teleoperation CLI Client
Connects to LumiRover over USB Serial or HC-05/HC-06 Bluetooth SPP to send
movement commands and monitor live ultrasonic/hazard sensor diagnostics.

Usage:
    python tools/telemetry_cli.py --port COM3 --baud 9600
    python tools/telemetry_cli.py --simulate
"""

import argparse
import sys
import time
from dataclasses import dataclass


@dataclass
class TelemetryFrame:
    timestamp: float
    distance_cm: float
    hazard_detected: bool
    active_command: str
    state: str = "MANUAL_TELEOP"


VALID_COMMANDS = {
    "F": "Move Forward",
    "B": "Move Backward",
    "L": "Turn Left",
    "R": "Turn Right",
    "Q": "Strafe Left",
    "E": "Strafe Right",
    "S": "Stop All Motors",
}


def run_simulation_loop() -> None:
    """Run a mock telemetry loop for testing without physical rover hardware."""
    print("[LumiRover CLI] Starting in hardware-free SIMULATION mode...")
    print("Available commands:", ", ".join(f"{k} ({v})" for k, v in VALID_COMMANDS.items()))
    print("Press Ctrl+C to exit.\n")

    simulated_distance = 65.0
    try:
        while True:
            cmd = input("lumirover> ").strip().upper()
            if not cmd:
                continue
            if cmd in ("EXIT", "QUIT"):
                break
            if cmd not in VALID_COMMANDS:
                print(f"  [!] Unknown command '{cmd}'. Valid keys: {list(VALID_COMMANDS.keys())}")
                continue

            # Simulate distance change based on movement
            if cmd == "F":
                simulated_distance = max(10.0, simulated_distance - 8.5)
            elif cmd == "B":
                simulated_distance = min(200.0, simulated_distance + 8.5)

            hazard = simulated_distance <= 20.0
            frame = TelemetryFrame(
                timestamp=time.time(),
                distance_cm=simulated_distance,
                hazard_detected=hazard,
                active_command=cmd,
                state="OBSTACLE_STOP" if hazard else "MANUAL_TELEOP",
            )
            print(
                f"  -> Sent '{cmd}' ({VALID_COMMANDS[cmd]}) | "
                f"Dist: {frame.distance_cm:.1f} cm | "
                f"State: {frame.state}"
            )
    except KeyboardInterrupt:
        print("\n[LumiRover CLI] Session closed.")


def main() -> int:
    parser = argparse.ArgumentParser(description="LumiRover Serial/Bluetooth Telemetry CLI")
    parser.add_argument("--port", type=str, default=None, help="Serial port (e.g., COM3 or /dev/ttyUSB0)")
    parser.add_argument("--baud", type=int, default=9600, help="Serial baud rate (default: 9600)")
    parser.add_argument("--simulate", action="store_true", help="Run in offline simulation mode")
    args = parser.parse_args()

    if args.simulate or not args.port:
        if not args.port and not args.simulate:
            print("[Info] No --port specified; defaulting to --simulate mode.\n")
        run_simulation_loop()
        return 0

    try:
        import serial  # type: ignore
    except ImportError:
        print("Error: pyserial is not installed. Run: pip install pyserial", file=sys.stderr)
        return 1

    print(f"[LumiRover CLI] Connecting to {args.port} at {args.baud} baud...")
    with serial.Serial(args.port, args.baud, timeout=0.5) as ser:
        time.sleep(2.0)  # Wait for Arduino bootloader reset
        print("[LumiRover CLI] Connected! Type command characters (F/B/L/R/Q/E/S) or 'quit'.")
        while True:
            cmd = input("lumirover> ").strip().upper()
            if cmd in ("EXIT", "QUIT"):
                break
            if cmd in VALID_COMMANDS:
                ser.write(cmd.encode("ascii"))
                response = ser.readline().decode("utf-8", errors="replace").strip()
                if response:
                    print(f"  [Telemetry] {response}")
            else:
                print(f"  [!] Invalid command. Choose from: {list(VALID_COMMANDS.keys())}")

    return 0


if __name__ == "__main__":
    sys.exit(main())
