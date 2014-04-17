/*
 * Split spinlock implementation out into its own file, so it can be
 * compiled in a FTRACE-compatible way.
 */
#include <linux/spinlock.h>
#include <linux/module.h>
#include <linux/jump_label.h>

#include <asm/paravirt.h>

#ifdef CONFIG_PARAVIRT_SPINLOCKS
struct pv_lock_ops pv_lock_ops = {
#ifdef CONFIG_SMP
	.lock_spinning = __PV_IS_CALLEE_SAVE(paravirt_nop),
	.unlock_kick = paravirt_nop,
#endif
};
EXPORT_SYMBOL(pv_lock_ops);

struct static_key paravirt_ticketlocks_enabled = STATIC_KEY_INIT_FALSE;
EXPORT_SYMBOL(paravirt_ticketlocks_enabled);
#endif

#ifdef CONFIG_PARAVIRT_UNFAIR_LOCKS
struct static_key paravirt_unfairlocks_enabled = STATIC_KEY_INIT_FALSE;
EXPORT_SYMBOL(paravirt_unfairlocks_enabled);

#include <linux/init.h>
#include <asm/cpufeature.h>

/*
 * Enable unfair lock only if it is running under a hypervisor
 */
static __init int unfair_locks_init_jump(void)
{
	if (!boot_cpu_has(X86_FEATURE_HYPERVISOR))
		return 0;

	static_key_slow_inc(&paravirt_unfairlocks_enabled);
	printk(KERN_INFO "Unfair spinlock enabled\n");

	return 0;
}
early_initcall(unfair_locks_init_jump);

#endif
