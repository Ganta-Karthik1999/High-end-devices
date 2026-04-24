use std::fs::OpenOptions;
use std::io::{Read, Write};
use std::thread::sleep;
use std::time::Duration;

fn write_command(cmd: &[u8]) {
    let mut file = OpenOptions::new()
        .write(true)
        .open("/dev/myleds")
        .expect("Failed to open /dev/myleds for writing");

    file.write_all(cmd).expect("Write failed");
    file.flush().expect("Flush failed");
}

fn read_status() {
    let mut file = OpenOptions::new()
        .read(true)
        .open("/dev/myleds")
        .expect("Failed to open /dev/myleds for reading");

    let mut buffer = [0u8; 32];
    let n = file.read(&mut buffer).expect("Read failed");

    print!("{}", String::from_utf8_lossy(&buffer[..n]));
}

fn main() {
    loop {
        write_command(b"led=on");
        read_status();
        sleep(Duration::from_millis(100));

        write_command(b"led=off");
        read_status();
        sleep(Duration::from_millis(100));
    }
}