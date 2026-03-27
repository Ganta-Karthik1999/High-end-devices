
/*
    This function handles the call by value
    params : x (passing by value)
    return : returning the value (u8)
*/
fn by_value(mut x:u8)->u8{

    let bit2 :u8 = (x>>1)&1;
    let bit4 :u8 = (x>>3)&1;

    if bit2^bit4 == 1{
        x = x ^ ((1<<1)|(1<<3));
    }
    
    x

}

/*
    This function handles the call by reference
    params : x (pasing by address)
    return : not return value
*/
fn by_reference(x:&mut u8){
    let bit2 :u8 = (*x>>1)&1;
    let bit4 :u8 = (*x>>3)&1;

    if bit2^bit4 == 1{
        *x = *x ^ ((1<<1)|(1<<3));
    }
}


fn main(){
    let mut sample:u8;
    for i in 0x0..0xff{
        // calling my value
        println!("Original value->{:#x} by_value->{:#x}",i,by_value(i));
       // Assigning a temp variable
        sample = i;
        // calling by reference
        by_reference(&mut sample);
        println!("Original value->{:#x} by_reference->{:#x}",i,sample);
        
    }



}