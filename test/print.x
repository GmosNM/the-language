

fn main(): int{
    let x: int = 10;
    if (x = 10){
        println("x is {}", "10");
        # TODO: add support for variable reference in the println
        println("100%", "");
        return x;
    }else{
        println("x is {}", "not 10");
        return 404;
    }
    return 0;
}
