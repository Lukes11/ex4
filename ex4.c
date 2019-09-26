#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/seq_file.h>
#include <linux/proc_fs.h>

static char *int_str;

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Luke Sanyour");
MODULE_DESCRIPTION("LKP Exercise 4");


module_param(int_str, charp, S_IRUSR | S_IRGRP | S_IROTH);


MODULE_PARM_DESC(int_str, "A comma-separated list of integers");


static LIST_HEAD(mylist);

struct entry {
	int val;
	struct list_head list;
};
static char linkedList[50];
static int store_value(int val)
{ 
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
	
	struct entry  *current_entry;
	char structureValues[50];
	char name[20] = "Linked List: ";
	sprintf(structureValues, "%s", name);
	list_for_each_entry(current_entry, &mylist, list)
	{
		sprintf(structureValues,"%d, ", current_entry->val);
	}
	strcpy(linkedList, structureValues);
}


static void destroy_linked_list_and_free(void)
{
	
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


	
	params = kstrdup(int_str, GFP_KERNEL);
	if (!params)
		return -ENOMEM;
	orig = params;

	
	while ((p = strsep(&params, ",")) != NULL) {
		if (!*p)
			continue;
		
		err = kstrtoint(p, 0, &val);
		if (err)
			break;

		
		err = store_value(val);
		if (err)
			break;
	}

	
	kfree(orig);
	return err;
}

static void run_tests(void)
{
	test_linked_list();
}

static void cleanup(void)
{
	
	printk(KERN_INFO "\nCleaning up...\n");

	destroy_linked_list_and_free();
}

static int __init ex4_init(void)
{
	int err = 0;

	
	if (!int_str) {
		printk(KERN_INFO "Missing \'int_str\' parameter, exiting\n");
		return -1;
	}

	
	err = parse_params();
	if (err)
		goto out;

	
	run_tests();
out:
	
	cleanup();
	return err;
}
static void __exit ex4_exit(void)
{
	
	return;
}

//Create proc entry
static int structures_proc_show(struct seq_file *m, void *v) {
  seq_printf(m, "%s\n", linkedList);
  return 0;
}

static int structures_proc_open(struct inode *inode, struct  file *file) {
  return single_open(file, structures_proc_show, NULL);
}

static const struct file_operations structures_proc_fops = {
  .owner = THIS_MODULE,
  .open = structures_proc_open,
  .read = seq_read,
  .llseek = seq_lseek,
  .release = single_release,
};

static int __init structures_proc_init(void) {
  proc_create("structures_proc", 0, NULL, &structures_proc_fops);
  return 0;
}

static void __exit structures_proc_exit(void) {
  remove_proc_entry("structures_proc", NULL);
}

//exit module
module_init(ex4_init);

module_exit(ex4_exit);
