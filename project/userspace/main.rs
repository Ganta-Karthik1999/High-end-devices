use std::fs::OpenOptions;
use std::io::{Read, Write};
use std::thread;
use std::time::Duration;

const DEVICE_FILE: &str = "/dev/project";

fn read_speed() -> i32 {
    let mut file = match OpenOptions::new().read(true).open(DEVICE_FILE) {
        Ok(file) => file,
        Err(e) => {
            eprintln!("Failed to open device for reading: {}", e);
            return 0;
        }
    };

    let mut buffer = String::new();

    if let Err(e) = file.read_to_string(&mut buffer) {
        eprintln!("Failed to read speed: {}", e);
        return 0;
    }

    let buffer = buffer.trim();

    if buffer.starts_with("speed=") {
        let value = buffer.replace("speed=", "");
        return value.parse::<i32>().unwrap_or(0);
    }

    0
}

fn write_led_duty(l1: i32, l2: i32) {
    let mut file = match OpenOptions::new().write(true).open(DEVICE_FILE) {
        Ok(file) => file,
        Err(e) => {
            eprintln!("Failed to open device for writing: {}", e);
            return;
        }
    };

    let command = format!("L1={} L2={}\n", l1, l2);

    if let Err(e) = file.write_all(command.as_bytes()) {
        eprintln!("Failed to write LED duty cycle: {}", e);
    }
}

fn map_speed_to_leds(speed: i32) -> (i32, i32) {
    if speed <= 0 {
        (10, 0)
    } else if speed <= 5 {
        (25, 0)
    } else if speed <= 10 {
        (50, 20)
    } else if speed <= 20 {
        (75, 50)
    } else {
        (100, 100)
    }
}

fn main() {
    println!("Rust userspace program started");
    println!("Reading speed from /dev/project");
    println!("Writing LED duty cycle to /dev/project");

    loop {
        let speed = read_speed();

        let (l1, l2) = map_speed_to_leds(speed);

        println!("speed={} presses/10s -> L1={}%, L2={}%", speed, l1, l2);

        write_led_duty(l1, l2);

        thread::sleep(Duration::from_secs(1));
    }
}