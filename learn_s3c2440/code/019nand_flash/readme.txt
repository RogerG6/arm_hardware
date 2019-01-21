001. nand_init/read id
002. put id details
003. relocate nand flash 's code to sdram
004. add do_read_nand function
005. add erase and write function
006. add nand_block 判断函数，实现了read skip bad block
007. 实现了：如果遇到坏块，跳到下一个block首地址read
008. 实现了：如果遇到坏块，跳到下一个block首地址write

