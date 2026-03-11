

fn main (){
    // Set the 3rd bit of a 16 bit word
    let value1:u16 = 0x2105; 
    let result1 = value1 | (1<<2);// 
    println!("Value before : {:#x} and after setting the 3rd bit of 16-bit word :{:#x}",value1,result1);

    // Set the 4th bit of a 16 bit word 
    let value2:u16 = 0x21ff;
    let result2:u16 = value2 |(1<<3);
    println!("Value before :{:#x} after setting the 4th bit of the 16 bit  :{:#x}",value2,result2);

    // Set the 5th bit of a 16 bit word 
    let value3:u16 = 0x2100;
    let result3:u16 = value3 | (1<<4);
    println!("Value before :{:#x} after setting the 5th bit of the 16 bit  :{:#x}",value3,result3);

    // Set the 2nd and 5th bit at the sametime
    let value4:u16 = 0x80;
    let result4 = value4 |((1<<1) | (1<<4));
    println!("Value before :{:#x} after setting the 2th bit of the 5 bit  :{:#x}",value4,result4);

    // Clear the bit 1st and 7th at the sametime 
    let value5:u16 =0xff;
    let result5 = value5 & !((1<<0) | (1<<6));
    println!("Value before : {:#x} and after clearing the 1st and 7th bit  :{:#x}",value5,result5);


    // Flip the bit 3rd and 4th bit at the sametime 
    let value6:u16 =0x00;
    let result6 = value6 ^ ((1<<2) | (1<<3));
    println!("Value before : {:#x} and after fliping the bits 3rd and 4th bit  :{:#x}",value6,result6);
      

}