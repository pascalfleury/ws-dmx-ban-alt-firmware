#ifndef STC8H_H
#define STC8H_H

/* STC8H1K16 specific registers */

/* P_SW2 is defined in stc15w.h */
#define EAXFR 0x80  /* Enable access to extended SFR (bit 7) */

/* Extended SFR access macros for PWMA registers */
/* These are accessed when EAXFR bit is set in P_SW2 */
#define PWMA_CCER1  (*(unsigned char volatile __xdata *)0xFEB1)
#define PWMA_CCER2  (*(unsigned char volatile __xdata *)0xFEB2)
#define PWMA_CCMR1  (*(unsigned char volatile __xdata *)0xFEB3)
#define PWMA_CCMR2  (*(unsigned char volatile __xdata *)0xFEB4)
#define PWMA_CCMR3  (*(unsigned char volatile __xdata *)0xFEB5)
#define PWMA_CCMR4  (*(unsigned char volatile __xdata *)0xFEB6)
#define PWMA_CNTL   (*(unsigned char volatile __xdata *)0xFEB7)
#define PWMA_CNTH   (*(unsigned char volatile __xdata *)0xFEB8)
#define PWMA_PSCRH  (*(unsigned char volatile __xdata *)0xFEB9)
#define PWMA_PSCRL  (*(unsigned char volatile __xdata *)0xFEBA)
#define PWMA_ARRL   (*(unsigned char volatile __xdata *)0xFEBB)
#define PWMA_ARRH   (*(unsigned char volatile __xdata *)0xFEBC)
#define PWMA_CCR1L  (*(unsigned char volatile __xdata *)0xFEBD)
#define PWMA_CCR1H  (*(unsigned char volatile __xdata *)0xFEBE)
#define PWMA_CCR2L  (*(unsigned char volatile __xdata *)0xFEBF)
#define PWMA_CCR2H  (*(unsigned char volatile __xdata *)0xFEC0)
#define PWMA_CCR3L  (*(unsigned char volatile __xdata *)0xFEC1)
#define PWMA_CCR3H  (*(unsigned char volatile __xdata *)0xFEC2)
#define PWMA_CCR4L  (*(unsigned char volatile __xdata *)0xFEC3)
#define PWMA_CCR4H  (*(unsigned char volatile __xdata *)0xFEC4)
#define PWMA_BKR    (*(unsigned char volatile __xdata *)0xFEC5)
#define PWMA_DTR    (*(unsigned char volatile __xdata *)0xFEC6)
#define PWMA_OISR   (*(unsigned char volatile __xdata *)0xFEC7)
#define PWMA_CR1    (*(unsigned char volatile __xdata *)0xFEC8)
#define PWMA_IER    (*(unsigned char volatile __xdata *)0xFEC9)
#define PWMA_SR1    (*(unsigned char volatile __xdata *)0xFECA)
#define PWMA_ENO    (*(unsigned char volatile __xdata *)0xFECB)
#define PWMA_PS     (*(unsigned char volatile __xdata *)0xFECC)
#define PWMA_IOFR   (*(unsigned char volatile __xdata *)0xFECD)

/* UART1 DMA registers (extended SFR, need EAXFR) */
#define DMA_UR1T_CFG   (*(unsigned char volatile __xdata *)0xFA00)
#define DMA_UR1T_CR    (*(unsigned char volatile __xdata *)0xFA01)
#define DMA_UR1T_STA   (*(unsigned char volatile __xdata *)0xFA02)
#define DMA_UR1T_AMT   (*(unsigned char volatile __xdata *)0xFA03)
#define DMA_UR1T_DONE  (*(unsigned char volatile __xdata *)0xFA04)
#define DMA_UR1T_TXAH  (*(unsigned char volatile __xdata *)0xFA05)
#define DMA_UR1T_TXAL  (*(unsigned char volatile __xdata *)0xFA06)

#define DMA_UR1R_CFG   (*(unsigned char volatile __xdata *)0xFA08)
#define DMA_UR1R_CR    (*(unsigned char volatile __xdata *)0xFA09)
#define DMA_UR1R_STA   (*(unsigned char volatile __xdata *)0xFA0A)
#define DMA_UR1R_AMT   (*(unsigned char volatile __xdata *)0xFA0B)
#define DMA_UR1R_DONE  (*(unsigned char volatile __xdata *)0xFA0C)
#define DMA_UR1R_RXAH  (*(unsigned char volatile __xdata *)0xFA0D)
#define DMA_UR1R_RXAL  (*(unsigned char volatile __xdata *)0xFA0E)

/* DMA interrupt vector number */
#define DMA_UR1R_VECTOR 13

#endif
