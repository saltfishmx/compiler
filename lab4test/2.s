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

fact:  move $8, $a0
  li $25, 1
  beq $8, $25, label0
  j label1
label0:
  move $v0, $8
  jr $ra
  j label2
label1:
  addi $9, $8, -1
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  addi $sp, $sp, -8
  sw $8, 0($sp)
  sw $9, 4($sp)
  addi $sp, $sp, -4
  sw $a0, 0($sp)
  move $a0, $9
  jal fact
  lw $a0, 0($sp)
  addi $sp, $sp, 4
  lw $8, 0($sp)
  lw $9, 4($sp)
  addi $sp, $sp, 8
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  move $10, $v0
  mul $11, $8, $10
  move $v0, $11
  jr $ra
label2:

main:
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal read
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  move $8, $v0
  move $9, $8
  li $25, 1
  bgt $9, $25, label3
  j label4
label3:
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  addi $sp, $sp, -8
  sw $8, 0($sp)
  sw $9, 4($sp)
  addi $sp, $sp, -4
  sw $a0, 0($sp)
  move $a0, $9
  jal fact
  lw $a0, 0($sp)
  addi $sp, $sp, 4
  lw $8, 0($sp)
  lw $9, 4($sp)
  addi $sp, $sp, 8
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  move $10, $v0
  move $11, $10
  j label5
label4:
  li $11, 1
label5:
  addi $sp, $sp, -8
  sw $ra, 0($sp)
  sw $a0, 4($sp)
  move $a0, $11 
  jal write
  lw $ra, 0($sp)
  lw $a0, 4($sp)
  addi $sp, $sp, 8

  li $25, 0
  move $v0, $25
  jr $ra
