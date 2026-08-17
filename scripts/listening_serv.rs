

//just for tests

use std::net::TcpListener;


fn main(){

let listener = TcpListener::bind("127.0.0.1:8081").unwrap();
    for _stream in listener.incoming(){
        
    }
}