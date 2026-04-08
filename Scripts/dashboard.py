import serial
import time
from rich.console import Console
from rich.table import Table
from rich.live import Live
from rich.text import Text
from datetime import datetime

# --- CONFIGURATION ---
# Change this to your STM32's COM port (e.g., "COM3" for Windows, "/dev/ttyACM0" for Mac/Linux)
SERIAL_PORT = "COM12"
BAUD_RATE = 115200

console = Console()

# Dictionary to store the latest data for each tracker
tracker_data = {}

def generate_table() -> Table:
    """Creates a Rich table based on the current tracker_data dictionary."""
    table = Table(title="📡 LoRaWAN Fleet Dashboard", style="cyan")

    table.add_column("Tracker ID", justify="center", style="bold white")
    table.add_column("Status", justify="center", style="bold")
    table.add_column("Latitude", justify="right", style="magenta")
    table.add_column("Longitude", justify="right", style="magenta")
    table.add_column("Last Seen", justify="right", style="dim")

    for t_id, data in sorted(tracker_data.items()):
        # Handle the visual styling based on the emergency flag
        if data["status"] == "EMERGENCY":
            # Blinking red alert!
            status_text = Text("🚨 EMERGENCY", style="bold blink red on white")
            row_style = "red"
        else:
            status_text = Text("✅ NORMAL", style="bold green")
            row_style = "white"

        table.add_row(
            f"#{t_id}",
            status_text,
            f"{data['lat']:.6f}",
            f"{data['lon']:.6f}",
            data['last_seen'],
            style=row_style
        )
    return table

def main():
    try:
        # Initialize Serial Connection
        SERIAL_PORT = input(f"Input the Serial port name: ")
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
        console.print(f"[bold green]Successfully connected to {SERIAL_PORT}[/bold green]")
        console.print("Waiting for Ground Station data...\n")

    except Exception as e:
        console.print(f"[bold red]Failed to connect to {SERIAL_PORT}. Is it plugged in? Error: {e}[/bold red]")
        return

    # Start the live-updating display
    with Live(generate_table(), refresh_per_second=4) as live:
        while True:
            try:
                if ser.in_waiting > 0:
                    # Read the raw bytes from the STM32 and decode to a string
                    raw_line = ser.readline().decode('utf-8').strip()

                    # Ignore completely empty lines
                    if not raw_line:
                        continue

                    # Example expected string: "1,7.311640,125.674220,EMERGENCY"
                    parts = raw_line.split(',')

                    # Ensure we got exactly 4 pieces of data AND the first piece is a number
                    if len(parts) == 4 and parts[0].isdigit():
                        t_id = int(parts[0])
                        lat = float(parts[1])
                        lon = float(parts[2])
                        status = parts[3]

                        # Update the dictionary with the fresh data
                        tracker_data[t_id] = {
                            "lat": lat,
                            "lon": lon,
                            "status": status,
                            "last_seen": datetime.now().strftime("%H:%M:%S")
                        }

                        # The rich Live view will automatically refresh the table!
                        live.update(generate_table())
                        
                    else:
                        # --- THE NEW DEBUG PRINTER ---
                        # If the string wasn't 4 CSV parts, it must be a printf debug message!
                        # We print it in a dim, italic, yellow font so it stands out from the table.
                        console.print(f"[dim italic yellow]STM32: {raw_line}[/dim italic yellow]")

            except KeyboardInterrupt:
                break # Allow user to cleanly exit with Ctrl+C
            except Exception as e:
                console.print(f"[bold red]Error processing serial data: {e}[/bold red]")
                # Ignore random serial decoding glitches that happen when plugging/unplugging
                pass

if __name__ == "__main__":
    main()