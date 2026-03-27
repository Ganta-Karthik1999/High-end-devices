
fn factorial(x:u32)->u32{

    if x!=0{
        return x*factorial(x-1)
    }
    1
}

fn main(){
    println!("{}",factorial(4))
}