use std::os::raw::c_char;
#[link(name = "b.o", kind = "static")]

#[no_mangle]
pub extern "C" fn foo(x:      *const c_char) {
	println!("Hi");
}
//#[link(name = "/home/a/proj/lib.o")]
extern {
fn main_() -> i64;
}
//extern unsafe fn main_();
fn main() {
	unsafe {
	main_();
}
}

