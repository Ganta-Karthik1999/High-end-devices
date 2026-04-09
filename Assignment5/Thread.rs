use::std::thread;
use::std::time::Duration;


fn thread3(){

    for i in 0..=10{
        println!("Thread 3 is running: {}", i);
        thread::sleep(Duration::from_secs(1));
    }

}


fn thread2(){

    let t3 = thread::spawn(||{
        thread3();
    });

    for i in 0..=10{
        println!("Thread 2 is running: {}", i);
        thread::sleep(Duration::from_secs(1));
    }

    t3.join().unwrap();

}


fn thread1(){

    let t2 = thread::spawn(||{
        thread2();
    });

    for i in 0..=10{
        println!("Thread 1 is running: {}", i);
        thread::sleep(Duration::from_secs(1));
    }

    t2.join().unwrap();


}


fn main(){

    let t1 = thread::spawn(||{
        thread1();
    
    });

    println!("Main thread is running");
    t1.join().unwrap();

}