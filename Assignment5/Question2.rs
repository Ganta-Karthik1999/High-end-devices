use std::thread;


fn sum4(a: i32, b: i32, c: i32, d: i32) -> i32 {
        let t1 = thread::spawn(move || {
            a + b
        });
        let t2 = thread::spawn(move || {
            c + d
        });
        let result1 = t1.join().unwrap();
        let result2 = t2.join().unwrap();
        result1 + result2
}




fn main() {

    for x in 0..=10 {
        let result = sum4(x,x,x,x);
        println!("{} x 4 = {}", x, result);
    }


}