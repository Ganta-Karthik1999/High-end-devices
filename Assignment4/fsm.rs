#[derive(Debug, Copy, Clone)]
enum State {
    A,
    B,
    C,
    D,
}

fn update(state: State, x: bool, y: bool)->(State,u8){
    match state{
        State::A =>{
            if x && !y {
                (State::B, 1)
            } else if x && y {
                (State::D, 2)
            } else {
                // (State::A, 0)
                panic!("Invalid input for state A");
            }
        }

        State::B =>{
            if !x && y {
                (State::A, 0)
            } else if x && !y {
                (State::C, 2)
            } else {
                // (State::B, 0)
                panic!("Invalid input for state B");
            }
        }

        State::C =>{
            if !x && y {
                (State::B, 1)
            } else if !x&& !y {
                (State::D, 2)
            } else {
                // (State::C, 0)
                panic!("Invalid input for state C");
            }
        }

        State::D =>{
            if !x && !y {
                (State::A, 0)
            } else if x && y {
                (State::C, 4)
            } else {
                // (State::D, 0)
                panic!("Invalid input for state D");
            }
        }
     }
}



fn main(){

    let mut state = State::A;
    
    let inputs = [
        (true, false),  // A -> B
        (true, false),  // B -> C
        (false, false), // C -> D
        (false, false), // D -> A
        (true, true),   // A -> D
        (true, true),   // D -> C
        (false, true),  // C -> B
        (false, true),  // B -> A
    ];

    println!("Initial state: {:?}", state);
    for (i,(x,y)) in inputs.iter().enumerate() {
        let (new_state, output) = update(state, *x, *y);
        println!("Input {}: Old State: {:?}, x={}, y={} -> Output: {}, New State: {:?}", i+1, state, x, y, output, new_state);
        state = new_state;
    }


}