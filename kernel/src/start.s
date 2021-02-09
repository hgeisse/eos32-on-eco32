;
; start.s -- startup and assembler support code
;

	.set	PAGE_SHIFT,12			; log2 of page size
	.set	PAGE_SIZE,1 << PAGE_SHIFT	; page size in bytes
	.set	PAGE_OFFSET,PAGE_SIZE - 1	; page offset mask
	.set	PAGE_NUMBER,~PAGE_OFFSET	; page number mask

	.set	PAGE_TABLE_BASE,0x80000000	; page table base address

	.set	U_PAGES,1			; size of u area in pages
	.set	U_SIZE,U_PAGES * PAGE_SIZE	; size of u area in bytes
	.set	U_BASE,0xC0000000 - U_SIZE	; begin of u area
	.set	U_KSTK_TOP,U_BASE + U_SIZE	; kernel stack top

	.set	ICNTXT_SIZE,34 * 4		; size of interrupt context

	.set	PSW,0				; reg # of PSW
	.set	V_SHIFT,27			; interrupt vector ctrl bit
	.set	V,1 << V_SHIFT
	.set	UM_SHIFT,26			; curr user mode ctrl bit
	.set	UM,1 << UM_SHIFT
	.set	PUM_SHIFT,25			; prev user mode ctrl bit
	.set	PUM,1 << PUM_SHIFT
	.set	OUM_SHIFT,24			; old user mode ctrl bit
	.set	OUM,1 << OUM_SHIFT
	.set	IE_SHIFT,23			; curr int enable ctrl bit
	.set	IE,1 << IE_SHIFT
	.set	PIE_SHIFT,22			; prev int enable ctrl bit
	.set	PIE,1 << PIE_SHIFT
	.set	OIE_SHIFT,21			; old int enable ctrl bit
	.set	OIE,1 << OIE_SHIFT

	.set	TLB_INDEX,1			; reg # of TLB Index
	.set	TLB_ENTRY_HI,2			; reg # of TLB EntryHi
	.set	TLB_ENTRY_LO,3			; reg # of TLB EntryLo
	.set	TLB_ENTRIES,32			; number of TLB entries
	.set	TLB_FIXED,4			; number of fixed entries

	.set	MONITOR_START,0xC0000000	; monitor's start entry point

;***************************************************************

	.import	main
	.import	trapISR
	.import	runrun
	.import	bootdsk

	.import	_bcode
	.import	_ecode
	.import	_bdata
	.import	_edata
	.import	_bbss
	.import	_ebss

	.export	proc0uarea
	.export	allocStart

	.export	idle

	.export	save
	.export	resume

	.export	savfp
	.export	restfp

	.export	flushTLB
	.export	setTLB

	.export	syncCaches

	.export	enableInt
	.export	disableInt
	.export	restoreInt

	.export	getMask
	.export	setMask

	.export	getISR
	.export	setISR

;***************************************************************

	.code
	.align	4

	; system bootstrap arrives here
bootstrap:
	j	start

	; interrupts and exceptions arrive here
interrupt:
	j	isr

	; user TLB misses arrive here
userMiss:
	.nosyn
	add	$26,$30,$0		; save ISR return address
					; why? - see ldw instruction below
	mvfs	$27,TLB_ENTRY_HI	; get page with missing translation
	slr	$27,$27,PAGE_SHIFT - 2	; 4 bytes per page table entry
	ldhi	$28,PAGE_TABLE_BASE	; compute pointer into page table
	or	$27,$27,$28
	ldw	$27,$27,0		; get page table entry
					; attention - this may miss again
					; then we arrive at 'interrupt' with
					; $26: original ISR return address
					; $27: pointer into page table
	mvts	$27,TLB_ENTRY_LO	; entry has the right format already
	tbwr				; write random into TLB
	add	$30,$26,$0		; restore return address
	rfx				; done
	.syn

;***************************************************************

	.code
	.align	4

	; general interrupt service routine
	; only register $28 is available for bootstrapping
isr:
	.nosyn
	mvfs	$28,PSW			; get PSW
	slr	$28,$28,PUM_SHIFT	; interrupted in user mode?
	and	$28,$28,1
	bne	$28,$0,setNewStk	; yes - setup new kernel stack
	mvfs	$28,PSW			; get PSW
	slr	$28,$28,16		; is this exception a TLB miss?
	and	$28,$28,0x1F
	sub	$28,$28,21
	bne	$28,$0,useCurrStk	; no - use current kernel stack
	mvfs	$28,PSW			; get PSW
	slr	$28,$28,OUM_SHIFT	; TLB miss triggered in user mode?
	and	$28,$28,1
	beq	$28,$0,useCurrStk	; no - use current kernel stack
setNewStk:
	ldhi	$28,U_KSTK_TOP		; setup new kernel stack
	or	$28,$28,U_KSTK_TOP
	j	stackSet
useCurrStk:
	add	$28,$29,$0		; current stack pointer is in $29
stackSet:
	sub	$28,$28,ICNTXT_SIZE	; $28 points to interrupt context
	stw	$0,$28,0*4		; save registers
	stw	$1,$28,1*4
	stw	$2,$28,2*4
	stw	$3,$28,3*4
	stw	$4,$28,4*4
	stw	$5,$28,5*4
	stw	$6,$28,6*4
	stw	$7,$28,7*4
	stw	$8,$28,8*4
	stw	$9,$28,9*4
	stw	$10,$28,10*4
	stw	$11,$28,11*4
	stw	$12,$28,12*4
	stw	$13,$28,13*4
	stw	$14,$28,14*4
	stw	$15,$28,15*4
	stw	$16,$28,16*4
	stw	$17,$28,17*4
	stw	$18,$28,18*4
	stw	$19,$28,19*4
	stw	$20,$28,20*4
	stw	$21,$28,21*4
	stw	$22,$28,22*4
	stw	$23,$28,23*4
	stw	$24,$28,24*4
	stw	$25,$28,25*4
	stw	$26,$28,26*4
	stw	$27,$28,27*4
	stw	$28,$28,28*4
	stw	$29,$28,29*4
	stw	$30,$28,30*4
	stw	$31,$28,31*4
	mvfs	$8,PSW			; save PSW
	stw	$8,$28,32*4
	mvfs	$9,TLB_ENTRY_HI		; save TLB EntryHi
	stw	$9,$28,33*4
	add	$29,$28,$0		; $29 is required to hold sp
	.syn
	sub	$29,$29,8		; reserve space for 2 arguments
	add	$5,$29,8		; $5 = pointer to interrupt context
	slr	$4,$8,16		; $4 = IRQ number
	and	$4,$4,0x1F
	sll	$8,$4,2			; $8 = 4 * IRQ number
	ldw	$8,$8,irqsrv		; get addr of service routine
	jalr	$8			; call service routine
	ldw	$8,$29,32*4+8		; check previous mode
	and	$8,$8,PUM		; was it kernel?
	beq	$8,$0,noResched		; yes - no rescheduling then
	ldbu	$8,$0,runrun		; else check rescheduling flag
	beq	$8,$0,noResched		; not set - no rescheduling
	add	$5,$29,8		; $5 = pointer to interrupt context
	add	$4,$0,32		; 32 = rescheduling pseudo exception
	jal	trapISR			; reschedule the CPU
noResched:
	add	$29,$29,8		; free space for 2 arguments
	.nosyn
	ldw	$8,$29,32*4		; restore PSW
	mvts	$8,PSW
	ldw	$9,$29,33*4		; restore TLB EntryHi
	mvts	$9,TLB_ENTRY_HI
	add	$28,$29,$0		; $28 points to interrupt context
	ldw	$0,$28,0*4		; restore registers
	ldw	$1,$28,1*4
	ldw	$2,$28,2*4
	ldw	$3,$28,3*4
	ldw	$4,$28,4*4
	ldw	$5,$28,5*4
	ldw	$6,$28,6*4
	ldw	$7,$28,7*4
	ldw	$8,$28,8*4
	ldw	$9,$28,9*4
	ldw	$10,$28,10*4
	ldw	$11,$28,11*4
	ldw	$12,$28,12*4
	ldw	$13,$28,13*4
	ldw	$14,$28,14*4
	ldw	$15,$28,15*4
	ldw	$16,$28,16*4
	ldw	$17,$28,17*4
	ldw	$18,$28,18*4
	ldw	$19,$28,19*4
	ldw	$20,$28,20*4
	ldw	$21,$28,21*4
	ldw	$22,$28,22*4
	ldw	$23,$28,23*4
	ldw	$24,$28,24*4
	ldw	$25,$28,25*4
	ldw	$26,$28,26*4
	ldw	$27,$28,27*4
	ldw	$29,$28,29*4
	ldw	$30,$28,30*4
	ldw	$31,$28,31*4
	ldw	$28,$28,28*4		; this must be done last
	rfx				; done
	.syn

;***************************************************************

	.code
	.align	4

	; startup
start:

	; force CPU into a defined state
	add	$8,$0,$0		; disable interrupts and user mode
	or	$8,$8,V			; let irq/exc vectors point to RAM
	mvts	$8,PSW

	; steal exceptions from machine monitor
	add	$8,$0,(0x2A << 26) | ((0x10000 - 4) >> 2)
	add	$9,$0,MONITOR_START
	stw	$8,$9,4			; MONITOR_START + 4: j exception
	stw	$8,$9,8			; MONITOR_START + 8: j user_miss
	ccs							; sync caches

	; initialize TLB
	jal	flushAll		; invalidate all TLB entries

	; copy data segment
	add	$10,$0,_bdata		; lowest dst addr to be written to
	add	$8,$0,_edata		; one above the top dst addr
	sub	$9,$8,$10		; $9 = size of data segment
	add	$9,$9,_ecode		; data is waiting right after code
	j	cpytest
cpyloop:
	ldw	$11,$9,0		; src addr in $9
	stw	$11,$8,0		; dst addr in $8
cpytest:
	sub	$8,$8,4			; downward
	sub	$9,$9,4
	bgeu	$8,$10,cpyloop

	; clear bss segment
	add	$8,$0,_bbss		; start with first word of bss
	add	$9,$0,_ebss		; this is one above the top
	j	clrtest
clrloop:
	stw	$0,$8,0			; dst addr in $8
	add	$8,$8,4			; upward
clrtest:
	bltu	$8,$9,clrloop

	; clear u area of process 0
	add	$8,$0,_ebss		; determine first free memory frame
	add	$8,$8,PAGE_SIZE - 1
	and	$8,$8,PAGE_NUMBER
	stw	$8,$0,proc0uarea	; which hosts u area of proc 0
	add	$9,$8,U_SIZE		; frame allocation starts on top
	stw	$9,$0,allocStart
clrp0u:
	stw	$0,$8,0
	add	$8,$8,4			; upward
	bltu	$8,$9,clrp0u

	; map u area of process 0
	add	$4,$0,0			; index = 0 (fixed entry)
	add	$5,$0,U_BASE		; virtual address of u area
	ldw	$6,$0,proc0uarea	; get direct address of u area
	and	$6,$6,~0xC0000000	; convert to physical address
	or	$6,$6,0x00000003	; page is write enabled and valid
	jal	setTLB			; map the page

	; setup kernel stack for process 0
	add	$29,$0,U_KSTK_TOP	; sp gets decremented before write

	; store boot disk
	stw	$16,$0,bootdsk

	; call main
	jal	main			; call main, fork off init process

	; begin executing init process
	mvfs	$8,PSW
	or	$8,$8,PUM | PIE		; user mode, interrupts enabled
	mvts	$8,PSW
	add	$30,$0,$0		; at location 0
	rfx				; lift off to userland

	.data
	.align	4

proc0uarea:
	.word	0			; direct address of proc 0 u area

allocStart:
	.word	0			; direct address of first free frame

;***************************************************************

	.code
	.align	4

	; void idle(void)
	; this routine should wait for an interrupt and
	; return to its caller when the ISR has finished
	; if the processor doesn't have such an instruction,
	; it suffices to enable all interrupts for a short
	; period and then immediately return, PROVIDED THAT
	; THIS ROUTINE IS ONLY CALLED FROM WITHIN LOOPS
	; WHICH DO NOTHING IF NO INTERRUPT HAS OCCURRED
idle:
	mvfs	$8,PSW
	or	$9,$8,0xFFFF
	mvts	$9,PSW
	mvts	$8,PSW
	jr	$31

;***************************************************************

	.code
	.align	4

	; Bool save(label_t regs)
	; save process state to an array of registers
	; located in the currently mapped u area
	; return FALSE
save:
	stw	$16,$4,0*4		; save registers
	stw	$17,$4,1*4
	stw	$18,$4,2*4
	stw	$19,$4,3*4
	stw	$20,$4,4*4
	stw	$21,$4,5*4
	stw	$22,$4,6*4
	stw	$23,$4,7*4
	stw	$29,$4,8*4
	stw	$31,$4,9*4
	add	$2,$0,0			; return value is FALSE
	jr	$31

	; void resume(int addr, label_t regs)
	; restore process state from an array of registers
	; located in the u area with address addr
	; return TRUE (not to the caller of resume, of course,
	; but to the former caller of the corresponding save)
resume:
	mvfs	$8,PSW			; disable interrupts
	and	$8,$8,~IE
	mvts	$8,PSW
	add	$16,$5,$0		; save ptr to register array
	sll	$6,$4,PAGE_SHIFT	; map new u area
	or	$6,$6,0x03
	add	$5,$0,U_BASE
	add	$4,$0,0
	jal	setTLB			; $31 is allowed to be clobbered
	add	$8,$16,$0		; get pointer to register array
	ldw	$16,$8,0*4		; load registers
	ldw	$17,$8,1*4
	ldw	$18,$8,2*4
	ldw	$19,$8,3*4
	ldw	$20,$8,4*4
	ldw	$21,$8,5*4
	ldw	$22,$8,6*4
	ldw	$23,$8,7*4
	ldw	$29,$8,8*4
	ldw	$31,$8,9*4
	add	$2,$0,1			; return value is TRUE
	mvfs	$8,PSW			; enable interrupts
	or	$8,$8,IE
	mvts	$8,PSW
	jr	$31

;***************************************************************

	.code
	.align	4

	; void savfp(struct fps *fpsp)
	; save state of floating point hardware
	; but: we don't have such machinery...
savfp:
	jr	$31

	; void restfp(struct fps *fpsp)
	; restore state of floating point hardware
	; but: we don't have such machinery...
restfp:
	jr	$31

;***************************************************************

	.code
	.align	4

	; flush all TLB entries
flushAll:
	mvts	$0,TLB_ENTRY_LO		; invalidate all TLB entries
	add	$8,$0,0xC0000000	; by storing direct addresses
	add	$9,$0,0			; start at entry 0
	add	$10,$0,TLB_ENTRIES
	j	flushLoop

	; void flushTLB(void)
flushTLB:
	mvts	$0,TLB_ENTRY_LO		; invalidate non-fixed TLB entries
	add	$8,$0,0xC0000000 + TLB_FIXED * 0x1000
	add	$9,$0,TLB_FIXED		; spare fixed entries
	add	$10,$0,TLB_ENTRIES
flushLoop:
	mvts	$8,TLB_ENTRY_HI
	mvts	$9,TLB_INDEX
	tbwi
	add	$8,$8,0x1000		; all entries must be different
	add	$9,$9,1
	bne	$9,$10,flushLoop
	jr	$31

	; void setTLB(int index, unsigned int entryHi, unsigned int entryLo)
setTLB:
	mvts	$4,TLB_INDEX
	mvts	$5,TLB_ENTRY_HI
	mvts	$6,TLB_ENTRY_LO
	tbwi
	jr	$31

;***************************************************************

	.code
	.align	4

	; void syncCaches(void);
syncCaches:
	ccs							; invalidate icache, flush dcache
	jr	$31

;***************************************************************

	.code
	.align	4

	; Bool enableInt(void)
enableInt:
	mvfs	$8,PSW
	slr	$2,$8,IE_SHIFT
	and	$2,$2,1
	or	$8,$8,IE
	mvts	$8,PSW
	jr	$31

	; Bool disableInt(void)
disableInt:
	mvfs	$8,PSW
	slr	$2,$8,IE_SHIFT
	and	$2,$2,1
	and	$8,$8,~IE
	mvts	$8,PSW
	jr	$31

	; void restoreInt(Bool enable)
restoreInt:
	mvfs	$8,PSW
	bne	$4,$0,rstInt1
	and	$8,$8,~IE
	mvts	$8,PSW
	jr	$31
rstInt1:
	or	$8,$8,IE
	mvts	$8,PSW
	jr	$31

;***************************************************************

	.code
	.align	4

	; unsigned int getMask(void)
getMask:
	mvfs	$8,PSW
	and	$2,$8,0x0000FFFF	; return lower 16 bits only
	jr	$31

	; unsigned int setMask(unsigned int mask)
setMask:
	mvfs	$8,PSW
	and	$2,$8,0x0000FFFF	; return lower 16 bits only
	and	$4,$4,0x0000FFFF	; use lower 16 bits only
	and	$8,$8,0xFFFF0000
	or	$8,$8,$4
	mvts	$8,PSW
	jr	$31

;***************************************************************

	.code
	.align	4

	; ISR getISR(int irq)
getISR:
	sll	$4,$4,2
	ldw	$2,$4,irqsrv
	jr	$31

	; ISR setISR(int irq, ISR isr)
setISR:
	sll	$4,$4,2
	ldw	$2,$4,irqsrv
	stw	$5,$4,irqsrv
	jr	$31

	.data
	.align	4

irqsrv:
	.word	0			;  0: serial line 0 xmt interrupt
	.word	0			;  1: serial line 0 rcv interrupt
	.word	0			;  2: serial line 1 xmt interrupt
	.word	0			;  3: serial line 1 rcv interrupt
	.word	0			;  4: keyboard interrupt
	.word	0			;  5: unknown interrupt 5
	.word	0			;  6: unknown interrupt 6
	.word	0			;  7: unknown interrupt 7
	.word	0			;  8: disk interrupt
	.word	0			;  9: unknown interrupt 9
	.word	0			; 10: unknown interrupt 10
	.word	0			; 11: unknown interrupt 11
	.word	0			; 12: unknown interrupt 12
	.word	0			; 13: unknown interrupt 13
	.word	0			; 14: timer 0 interrupt
	.word	0			; 15: timer 1 interrupt
	.word	0			; 16: bus timeout exception
	.word	0			; 17: illegal instruction exception
	.word	0			; 18: privileged instruction exception
	.word	0			; 19: divide instruction exception
	.word	0			; 20: trap instruction exception
	.word	0			; 21: TLB miss exception
	.word	0			; 22: TLB write exception
	.word	0			; 23: TLB invalid exception
	.word	0			; 24: illegal address exception
	.word	0			; 25: privileged address exception
	.word	0			; 26: unknown exception 26
	.word	0			; 27: unknown exception 27
	.word	0			; 28: unknown exception 28
	.word	0			; 29: unknown exception 29
	.word	0			; 30: unknown exception 30
	.word	0			; 31: unknown exception 31
