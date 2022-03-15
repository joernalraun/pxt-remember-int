# remember-int

read and write int numbers to persistent flash memory.
the number of ints, which can be stored can be queried with getnumpages().
writeflashnum(page, intValue) writes the intValue into given page (1 based).
intValue = readflashnum(page) reads the intValue, which was stored in page.

## example 

this example starts counting from -1 to 0 the first call.
After reset it counts from 0 to 1.
power off and on again it counts from 1 to 2.
and so on...

```
basic.showString("alt:")
let value = flash.readflashnum(1)
basic.showNumber(value)
flash.writeflashnum(1, value + 1)
basic.showString("neu:")
value = flash.readflashnum(1)
basic.showNumber(value)
basic.pause(1000)
basic.clearScreen()
basic.showString("pages:")
basic.showNumber(flash.getnumpages())
```

## License

MIT

## Supported targets

* for PXT/microbit
* for PXT/calliope

