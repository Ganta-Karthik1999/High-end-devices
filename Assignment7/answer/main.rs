use std::thread;
use std::time;

extern "C" {
    fn memmap(address: u64) -> *mut u32;
    fn unmemmap();
}

fn main() {
    let pin: u32 = 2;

    unsafe {
        let ptr = memmap(0x1f00000000);

        if ptr.is_null() {
            unmemmap();
            std::process::exit(0);
        }

        let gpio_ctrl = ptr.add(0xD0000 / 4);
        let rio_set   = ptr.add(0xE2000 / 4);
        let rio_clr   = ptr.add(0xE3000 / 4);
        let rio_oe    = ptr.add(0xE0004 / 4);

        *gpio_ctrl.add((pin * 2 + 1) as usize) = 5;
        *rio_oe = 1 << pin;

        loop {
            *rio_set = 1 << pin;
            thread::sleep(time::Duration::from_millis(100));

            *rio_clr = 1 << pin;
            thread::sleep(time::Duration::from_millis(100));
        }
    }
}