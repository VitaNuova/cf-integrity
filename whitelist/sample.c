void readKey() {}

void encrypt() {
   readKey();
}

void decrypt() {
   readKey();
}

void operation2() {
   encrypt();
   decrypt();
}

void call() {}

void notify() {}

void operation1() {
   call();
   notify();
}

void display() {}

void loadUI() {
   display();
}

void main() {
   loadUI();
   operation1();
   operation2();
}
