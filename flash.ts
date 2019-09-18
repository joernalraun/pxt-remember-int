//% weight=70 icon="\u26AF" color=#EC7505 block="FLASH"
namespace flash {


     //% blockId=flash_writeflashnum
     //% block="Write Flash Number|page %page|value %value" 
     //% shim=flash::writeflashnum
     export function writeflashnum(page: number, value: number): void {
        console.log("sim:w("+page+","+value+")")
        return
     }


     //% blockId=flash_readflashnum
     //% block="Read Flash Number|page %page" 
     //% shim=flash::readflashnum
     export function readflashnum(page: number): number {
        console.log("sim:r("+page+")")
        return 15
     }


     //% blockId=flash_getnumpages
     //% block="Get number of pages" 
     //% shim=flash::getnumpages
     export function getnumpages(): number {
        console.log("sim:gnp()")
        return 15
     }


}
