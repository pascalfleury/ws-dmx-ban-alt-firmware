#ifndef STC8H_H
#define STC8H_H

/* STC8H1K16 specific SFR registers */

/* Port mode registers (PxM0: push-pull, PxM1: input mode) */
__sfr __at (0x94) P0M0;
__sfr __at (0x93) P0M1;
__sfr __at (0x92) P1M0;
__sfr __at (0x91) P1M1;
__sfr __at (0x95) P2M0;
__sfr __at (0x96) P2M1;
__sfr __at (0xb1) P3M0;
__sfr __at (0xb2) P3M1;
__sfr __at (0xc9) P5M0;
__sfr __at (0xca) P5M1;

/* Auxiliary register */
__sfr __at (0x8e) AUXR;

/* Peripheral port switch register 2 */
__sfr __at (0xba) P_SW2;

/* Timer 2 */
__sfr __at (0xd6) T2H;
__sfr __at (0xd7) T2L;

/* Port 5 (bit-addressable) */
__sfr __at (0xc8) P5;
__sbit __at (0xc8) P5_0;
__sbit __at (0xc9) P5_1;
__sbit __at (0xca) P5_2;
__sbit __at (0xcb) P5_3;
__sbit __at (0xcc) P5_4;
__sbit __at (0xcd) P5_5;

/* Extended SFR access */
#define EAXFR 0x80  /* Enable access to extended SFR (bit 7) */

/* Extended SFR access macros for PWMA registers */
/* These are accessed when EAXFR bit is set in P_SW2 */
#define PWMA_CCER1  (*(volatile __xdata unsigned char *)0xFEB1)
#define PWMA_CCER2  (*(volatile __xdata unsigned char *)0xFEB2)
#define PWMA_CCMR1  (*(volatile __xdata unsigned char *)0xFEB3)
#define PWMA_CCMR2  (*(volatile __xdata unsigned char *)0xFEB4)
#define PWMA_CCMR3  (*(volatile __xdata unsigned char *)0xFEB5)
#define PWMA_CCMR4  (*(volatile __xdata unsigned char *)0xFEB6)
#define PWMA_CNTL   (*(volatile __xdata unsigned char *)0xFEB7)
#define PWMA_CNTH   (*(volatile __xdata unsigned char *)0xFEB8)
#define PWMA_PSCRH  (*(volatile __xdata unsigned char *)0xFEB9)
#define PWMA_PSCRL  (*(volatile __xdata unsigned char *)0xFEBA)
#define PWMA_ARRL   (*(volatile __xdata unsigned char *)0xFEBB)
#define PWMA_ARRH   (*(volatile __xdata unsigned char *)0xFEBC)
#define PWMA_CCR1L  (*(volatile __xdata unsigned char *)0xFEBD)
#define PWMA_CCR1H  (*(volatile __xdata unsigned char *)0xFEBE)
#define PWMA_CCR2L  (*(volatile __xdata unsigned char *)0xFEBF)
#define PWMA_CCR2H  (*(volatile __xdata unsigned char *)0xFEC0)
#define PWMA_CCR3L  (*(volatile __xdata unsigned char *)0xFEC1)
#define PWMA_CCR3H  (*(volatile __xdata unsigned char *)0xFEC2)
#define PWMA_CCR4L  (*(volatile __xdata unsigned char *)0xFEC3)
#define PWMA_CCR4H  (*(volatile __xdata unsigned char *)0xFEC4)
#define PWMA_BKR    (*(volatile __xdata unsigned char *)0xFEC5)
#define PWMA_DTR    (*(volatile __xdata unsigned char *)0xFEC6)
#define PWMA_OISR   (*(volatile __xdata unsigned char *)0xFEC7)
#define PWMA_CR1    (*(volatile __xdata unsigned char *)0xFEC8)
#define PWMA_IER    (*(volatile __xdata unsigned char *)0xFEC9)
#define PWMA_SR1    (*(volatile __xdata unsigned char *)0xFECA)
#define PWMA_ENO    (*(volatile __xdata unsigned char *)0xFECB)
#define PWMA_PS     (*(volatile __xdata unsigned char *)0xFECC)
#define PWMA_IOFR   (*(volatile __xdata unsigned char *)0xFECD)

/* UART1 DMA registers (extended SFR, need EAXFR) */
#define DMA_UR1T_CFG   (*(volatile __xdata unsigned char *)0xFA00)
#define DMA_UR1T_CR    (*(volatile __xdata unsigned char *)0xFA01)
#define DMA_UR1T_STA   (*(volatile __xdata unsigned char *)0xFA02)
#define DMA_UR1T_AMT   (*(volatile __xdata unsigned char *)0xFA03)
#define DMA_UR1T_DONE  (*(volatile __xdata unsigned char *)0xFA04)
#define DMA_UR1T_TXAH  (*(volatile __xdata unsigned char *)0xFA05)
#define DMA_UR1T_TXAL  (*(volatile __xdata unsigned char *)0xFA06)

#define DMA_UR1R_CFG   (*(volatile __xdata unsigned char *)0xFA08)
#define DMA_UR1R_CR    (*(volatile __xdata unsigned char *)0xFA09)
#define DMA_UR1R_STA   (*(volatile __xdata unsigned char *)0xFA0A)
#define DMA_UR1R_AMT   (*(volatile __xdata unsigned char *)0xFA0B)
#define DMA_UR1R_DONE  (*(volatile __xdata unsigned char *)0xFA0C)
#define DMA_UR1R_RXAH  (*(volatile __xdata unsigned char *)0xFA0D)
#define DMA_UR1R_RXAL  (*(volatile __xdata unsigned char *)0xFA0E)

/* DMA interrupt vector number */
#define DMA_UR1R_VECTOR 13

#endif /* STC8H_H */
