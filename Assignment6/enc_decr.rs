use std::thread;

fn encrypt(data: String) -> String {
    let mut encrypted = String::new();
    for c in data.chars(){
        let mut value = c as u8;
        if value >= 65 && value <= 90 {
            value += 30;
        }
        else if value >= 97 && value <= 128 {
            value -= 30;
        }
        encrypted.push(value as char);
    }
    encrypted
}


fn decrypt(data: String) -> String {
    let mut decrypted = String::new();
    for c in data.chars(){
        let mut value = c as u8;
         if value >= 97 && value <= 128 {
            value -= 30;
        } else if value >= 65 && value <= 90 {
            value += 30;
        }
        decrypted.push(value as char);
    }
    decrypted
}

fn main(){

    /*Using the encrypt and decrypt functions only contains if-else loop*/
    let input = String::from("This is a test");

    let encrypted = encrypt(input.clone());
    println!("Encrypted: {}", encrypted);

    let decrypted = decrypt(encrypted);
    println!("Decrypted: {}", decrypted);

    /*Using the encrypt and decrypt functions using threads*/

    let encrypt_thread = thread::spawn(move || encrypt(input.clone()));
    let encrypted = encrypt_thread.join().unwrap();
    println!("Encrypted with thread: {}", encrypted);

    let decrypt_thread = thread::spawn(move || decrypt(encrypted));
    let decrypted = decrypt_thread.join().unwrap();
    println!("Decrypted with thread: {}", decrypted);

}