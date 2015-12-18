define restart
	#halting Quark and ARC
	monitor targets 0
	monitor reset halt
	monitor targets 1
	monitor reset halt

	#setting an access watchpoint in the
	#Sensor Subsystem Status (SS_STS) register
	#This one is going to be read by Quark, to know the ARC status,
	#just before starting it.
	monitor wp 0xb0800604 4 access
	monitor resume

	#waiting to hit watchpoint
	monitor wait_halt 20000
	#removing watchpoint
	monitor rwp 0xb0800604

	#setting ARC program counter to reset vector address
	#and a hard breakpoint on main procedure.
	monitor targets 0
	set $pc=__reset
	hb main
	stepi
	c
	#waiting to hit bp.
	monitor wait_halt 20000

	#resume Quark
	shell sleep 1
	monitor targets 1
	monitor halt
	monitor resume

	#halt ARC
	shell sleep 1
	monitor targets 0
	monitor halt
	c
end
