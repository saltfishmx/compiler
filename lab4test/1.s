.data
_prompt: .asciiz "Enter an integer:"
_ret: .asciiz "\n"
.globl main
.text
read:
  li $v0, 4
  la $a0, _prompt
  syscall
  li $v0, 5
  syscall
  jr $ra

write:
  li $v0, 1
  syscall
  li $v0, 4
  la $a0, _ret
  syscall
  move $v0, $0
  jr $ra

main:
  li $8, 0
  li $9, 1
  li $10, 0
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal read
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  move $11, $v0
  move $12, $11
label0:
  blt $10, $12, label1
  j label2
label1:
  add $13, $8, $9
  move $14, $13
  addi $sp, $sp, -8
  sw $ra, 0($sp)
  sw $a0, 4($sp)
  move $a0, $9 
  jal write
  lw $ra, 0($sp)
  lw $a0, 4($sp)
  addi $sp, $sp, 8

  move $8, $9
  move $9, $14
  addi $15, $10, 1
  move $10, $15
  j label0
label2:
  li $25, 0
  move $v0, $25
  jr $ra
