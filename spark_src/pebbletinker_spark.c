// Spark Function must be int(String s);

int on(String args)
{
    int pin = args.toInt();
    switch(pin) {
        case 0:
            pin = D0;
            break;
        case 1:
            pin = D1;
            break;
        case 2:
            pin = D2;
            break;
        case 3:
            pin = D3;
            break;
        case 4:
            pin = D4;
            break;
        case 5:
            pin = D5;
            break;
        case 6:
            pin = D6;
            break;
        case 7:
            pin = D7;
            break;
    }
    digitalWrite(pin, HIGH);
    
    return pin;
}

int off(String args)
{
    int pin = args.toInt();
    switch(pin) {
        case 0:
            pin = D0;
            break;
        case 1:
            pin = D1;
            break;
        case 2:
            pin = D2;
            break;
        case 3:
            pin = D3;
            break;
        case 4:
            pin = D4;
            break;
        case 5:
            pin = D5;
            break;
        case 6:
            pin = D6;
            break;
        case 7:
            pin = D7;
            break;
    }
    digitalWrite(pin, LOW);
    
    return pin;
}

void setup() 
{
    pinMode(D0, OUTPUT);
    pinMode(D1, OUTPUT);
    Spark.function("on", on);
    Spark.function("off", off);
}

void loop() 
{

}