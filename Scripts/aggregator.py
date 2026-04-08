import paho.mqtt.client as mqtt
from paho.mqtt.enums import CallbackAPIVersion
import json
import base64
import time
import threading

# --- CONFIGURATION ---
MQTT_BROKER = "127.0.0.1" 
MQTT_PORT = 1883
APPLICATION_ID = "2"      
GROUND_STATION_EUI = "7e5442a4684e16b8" # Note: If downlinks fail, try reversing this to b8164e68a442547e

# Tracker DevEUIs converted to LSB (Least Significant Byte) order
FLEET_MAP = {
    "0d0a23307894876f": 1,
    "1093cc26a85f8ee7": 2,
    "b34c3e6dc223a44a": 3
}

# Thread-safe buffer for bundling tracker data
bundle_buffer = bytearray()
buffer_lock = threading.Lock()

# --- MQTT CALLBACKS ---
def on_connect(client, userdata, flags, reason_code, properties):
    """Callback triggered when the client connects to the broker."""
    if reason_code == 0:
        print(f"[INFO] Successfully connected to ChirpStack MQTT broker at {MQTT_BROKER}:{MQTT_PORT}")
        # Subscribe to all uplink events in the specified application
        topic = f"application/{APPLICATION_ID}/device/+/event/up"
        client.subscribe(topic)
        print(f"[INFO] Subscribed to topic: {topic}")
    else:
        print(f"[ERROR] Failed to connect to MQTT broker. Reason code: {reason_code}")

def on_message(client, userdata, msg):
    """Callback triggered when a message is received from ChirpStack."""
    global bundle_buffer
    
    print(f"\n[DEBUG] --- New Message Received on {msg.topic} ---", flush=True)
    
    try:
        # Step 1: Parse the JSON payload
        payload = json.loads(msg.payload.decode('utf-8'))
        
        # Step 2: Extract the DevEUI directly from the topic string (Fix for ChirpStack v3)
        # Topic format: application/2/device/1093cc26a85f8ee7/event/up
        topic_parts = msg.topic.split('/')
        if len(topic_parts) >= 4:
            dev_eui = topic_parts[3]  
        else:
            print("[ERROR] Unrecognized topic structure.")
            return
            
        print(f"[DEBUG] Extracted Device EUI: {dev_eui}", flush=True)

        # Step 3: Check if the device is one of our trackers
        if dev_eui in FLEET_MAP:
            tracker_id = FLEET_MAP[dev_eui]
            print(f"[INFO] Matched Tracker ID: {tracker_id}", flush=True)
            
            # Step 4: Extract and Decode the Base64 GPS data
            b64_data = payload.get('data')
            if not b64_data:
                print(f"[WARN] No 'data' field found for Tracker {tracker_id}. Skipping.", flush=True)
                return
                
            raw_bytes = base64.b64decode(b64_data)
            
            # Step 5: Append to buffer if data is valid (8 bytes)
            if len(raw_bytes) == 8:
                with buffer_lock:
                    bundle_buffer.append(tracker_id)
                    bundle_buffer.extend(raw_bytes)
                    print(f"[INFO] Buffered Tracker {tracker_id}. Buffer size: {len(bundle_buffer)} bytes.", flush=True)
            else:
                print(f"[WARN] Tracker {tracker_id} sent {len(raw_bytes)} bytes. Expected 8. Discarding.", flush=True)
        else:
            print(f"[DEBUG] Ignored DevEUI {dev_eui} (Not in FLEET_MAP).", flush=True)
        
    except Exception as e:
        print(f"[ERROR] Error processing message: {repr(e)}", flush=True)

# --- THE 15-SECOND AGGREGATOR THREAD ---
def flush_buffer_to_ground_station():
    """Background thread that bundles and sends downlinks every 15 seconds."""
    global bundle_buffer
    
    while True:
        time.sleep(15) 
        
        with buffer_lock:
            if len(bundle_buffer) > 0:
                print(f"\n[INFO] === 15-Second Timer: Flushing {len(bundle_buffer) // 9} trackers ===")
                
                try:
                    # Convert the binary buffer back to Base64 for the downlink
                    b64_data = base64.b64encode(bundle_buffer).decode('utf-8')
                    
                    # Prepare the ChirpStack Command Downlink JSON
                    downlink_msg = {
                        "devEui": GROUND_STATION_EUI,
                        "confirmed": False, 
                        "fPort": 1,
                        "data": b64_data
                    }
                    
                    # Construct the command topic
                    topic = f"application/{APPLICATION_ID}/device/{GROUND_STATION_EUI}/command/down"
                    
                    # Send the downlink to the Ground Station
                    result = client.publish(topic, json.dumps(downlink_msg))
                    
                    if result.rc == mqtt.MQTT_ERR_SUCCESS:
                        print(f"[INFO] Downlink published for Ground Station {GROUND_STATION_EUI}.")
                    else:
                        print(f"[ERROR] Failed to publish MQTT message. Code: {result.rc}")
                        
                except Exception as e:
                    print(f"[CRITICAL] Downlink failure: {repr(e)}")
                
                finally:
                    # Always clear buffer to prepare for next 15-second window
                    bundle_buffer.clear()
            else:
                # No data received in the last 15 seconds
                pass

# --- MAIN EXECUTION ---
print("[INFO] Initializing Aggregator with MQTT v2.0 API...")

# Initialize the client with the required Version 2 Callback API
client = mqtt.Client(CallbackAPIVersion.VERSION2)
client.on_connect = on_connect
client.on_message = on_message

try:
    # Start the timer thread in the background
    timer_thread = threading.Thread(target=flush_buffer_to_ground_station, daemon=True)
    timer_thread.start()

    # Connect and enter the processing loop
    print(f"[INFO] Connecting to {MQTT_BROKER}...")
    client.connect(MQTT_BROKER, MQTT_PORT, 60)
    
    print("[INFO] Listening for tracker uplinks. Press Ctrl+C to stop.")
    client.loop_forever()

except KeyboardInterrupt:
    print("\n[INFO] Script stopped by user.")
except Exception as e:
    print(f"\n[CRITICAL] Fatal error: {repr(e)}")
finally:
    client.disconnect()