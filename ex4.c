#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>

static char *int_str;

/* [X1: point 1]
 * Explain following in here.
 * List information about new module
 */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Luke Sanyour");
MODULE_DESCRIPTION("LKP Exercise 4");

/* [X2: point 1]
 * Explain following in here.
 * declaring command line arguments to be passed to module
 */
module_param(int_str, charp, S_IRUSR | S_IRGRP | S_IROTH);

/* [X3: point 1]
 * Explain following in here.
 * Document a description of the arguments
 */
MODULE_PARM_DESC(int_str, "A comma-separated list of integers");

/* [X4: point 1]
 * Explain following in here.
 * define a new linked list
 */
static LIST_HEAD(mylist);

/* [X5: point 1]
 * Explain following in here.
 * defines a struct named entry, with two components, an integer and another struct
 */
struct entry {
	int val;
	struct list_head list;
};

static int store_value(int val)
{
	/* [X6: point 10]
	 * Allocate a struct entry of which val is val
	 * and add it to the tail of mylist.
	 * Return 0 if everything is successful.
	 * Otherwise (e.g., memory allocation failure),
	 * return corresponding error code in error.h (e.g., -ENOMEM).
	 */
	struct entry *e1 = kmalloc(sizeof(*e1), GFP_KERNEL);
	if(e1 == NULL)
	{
		return ENOMEM;
	}
	else 
	{
		e1->val = val;
		list_add_tail(&e1->list, &mylist);
		return 0;
	}
}

static void test_linked_list(void)
{
	/* [X7: point 10]
	 * Print out value of all entries in mylist.
	 */
	struct entry  *current_entry;
	list_for_each_entry(current_entry, &mylist, list)
	{
 		printk(KERN_INFO "Val: %d\n", current_entry->val);
	}


}


static void destroy_linked_list_and_free(void)
{
	/* [X8: point 10]
	 * Free all entries in mylist.
	 */
	struct entry *current_entry, *next;
	list_for_each_entry_safe(current_entry, next, &mylist, list) 
        {
 		list_del(&current_entry->list);
 		kfree(current_entry); 
	}

}


static int parse_params(void)
{
	int val, err = 0;
	char *p, *orig, *params;


	/* [X9: point 1]
	 * Explain following in here.
	 * This copies and allocates memory for the argument string contained in int_str,
	 * if it can't be allocated, returns an error. Then orig is set to point at that string.
	 */
	params = kstrdup(int_str, GFP_KERNEL);
	if (!params)
		return -ENOMEM;
	orig = params;

	/* [X10: point 1]
	 * Explain following in here.
	 * Strsep extracts tokes from strings, and returns a pointer to that token. So this loop executes as long
	 * as there are tokens to still be extracted. 
	 */
	while ((p = strsep(&params, ",")) != NULL) {
		if (!*p)
			continue;
		/* [X11: point 1]
		 * Explain following in here.
		 * This takes to token previously extracted and converts it into an integer, and stores it in val. The return value is 
		 * stored into err. It returns 0 on success, so if it returns something else, the loop breaks. 
		 */
		err = kstrtoint(p, 0, &val);
		if (err)
			break;

		/* [X12: point 1]
		 * Explain following in here.
		 * This calls the function that was defined before, it creates a list entry with the value val. It returns 0 upon success, so
		 * if it returns something else, the loop breaks.
		 */
		err = store_value(val);
		if (err)
			break;
	}

	/* [X13: point 1]
	 * Explain following in here.
	 * This frees the memory assoicated with the pointer orig, and returns err. err should be 0 if everything was 
	 * successful, or some other error value otherwise. 
	 */
	kfree(orig);
	return err;
}

static void run_tests(void)
{
	/* [X14: point 1]
	 * Explain following in here.
	 * This calls the function that was defined previously, and prints out all of the elements in the list.
	 */
	test_linked_list();
}

static void cleanup(void)
{
	/* [X15: point 1]
	 * Explain following in here.
	 * This calls another function that was defined previously, is destroys all entries in the linked list and frees all
	 * memory associated with it. It also prints an info message notifying that the list is being cleaned. 
	 */
	printk(KERN_INFO "\nCleaning up...\n");

	destroy_linked_list_and_free();
}

static int __init ex4_init(void)
{
	int err = 0;

	/* [X16: point 1]
	 * Explain following in here.
	 * This checks if the proper arguments have been passed, and prints an error message and returns an error value if they 
	 * have not.
	 */
	if (!int_str) {
		printk(KERN_INFO "Missing \'int_str\' parameter, exiting\n");
		return -1;
	}

	/* [X17: point 1]
	 * Explain following in here.
	 * This attempts to parse the parameter strings into integers.
	 * If unsucessful, the program jumps to exit code. 
	 */
	err = parse_params();
	if (err)
		goto out;

	/* [X18: point 1]
	 * Explain following in here.
	 * This runs the previously defined function, where all members of the list are
	 * printed out. 
	 */
	run_tests();
out:
	/* [X19: point 1]
	 * Explain following in here.
	 * This piece of code calls cleanup(), which deletes every element in the list and frees all memory, 
	 * and then returns an error value. 
	 */
	cleanup();
	return err;
}

static void __exit ex4_exit(void)
{
	/* [X20: point 1]
	 * Explain following in here.
	 * This function is to be called at the end of the module, and it dosen't need
	 * to do anything since everything has been cleaned up so it returns nothing. 
	 */
	return;
}

/* [X21: point 1]
 * Explain following in here.
 * This defines the begining of the module, 
 * which calls the initializing function, which executes the purpose of the module.
 */
module_init(ex4_init);

/* [X22: point 1]
 * Explain following in here.
 * This defines the end of the module, which calls an exiting function. This function returns nothing. 
 */
module_exit(ex4_exit);
