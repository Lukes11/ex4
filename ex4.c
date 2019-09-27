#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/seq_file.h>
#include <linux/proc_fs.h>
#include <linux/hashtable.h>

static char *int_str;

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Luke Sanyour");
MODULE_DESCRIPTION("LKP Exercise 4");


module_param(int_str, charp, S_IRUSR | S_IRGRP | S_IROTH);


MODULE_PARM_DESC(int_str, "A comma-separated list of integers");

//define list
static LIST_HEAD(mylist);
//define hash table
static DEFINE_HASHTABLE(myHashTable, 5);
//define red-black tree
struct my_rb_tree {
	struct rb_root root_node;
};

static struct my_rb_tree tree;

//list entry
struct entry {
	int val;
	struct list_head list;
};
//hash table entry
struct hashEntry {
	int val;
	struct hlist_node hash_list;
};
//reb black tree entry
struct rbEntry {
	int val;
	struct rb_node run_node;
};
static char linkedList[50], hashTable[50];
static int store_value(int val)
{ 
	struct entry *le1 = kmalloc(sizeof(*le1), GFP_KERNEL);
	struct hashEntry *he1 = kmalloc(sizeof(*he1), GFP_KERNEL);
	struct rbEntry *rb1 =  kmalloc(sizeof(*rb1), GFP_KERNEL);
	//add to linked list
	if(le1 == NULL || he1 == NULL)
	{
		return ENOMEM;
	}
	else 
	{
		le1->val = val;
		list_add_tail(&le1->list, &mylist);
		he1->val = val;
		hash_add(myHashTable, &he1->hash_list, he1->val);
		rb1->val = val;
		rb_insert_entry(tree, rbEntry); 
		return 0;
	}
}

//function to insert a value into the rb tree
static void rb_insert_entry(struct my_rb_tree *root, struct rbEntry *en)
{
	struct rb_node **link = &my_rb_tree->root_node.rb_node;
	struct rb_node *parent = NULL;
	struct hashEntry *entry;
	while(*link)
	{
		parent = *link;
		entry = rb_entry(parent, struct rbEntry, run_node);
		if(en->val > entry->val)
			link = &parent->rb_left;
		else
			link = &parent->rb_right;
	}
	rb_link_node(&en->run_node, parent, link);
	rb_insert_color(&en->run_node, &root->root_node);
}

static void test_linked_list(void)
{
	
	struct entry  *current_entry;
	char structureValues[50];
	char name[20] = "Linked List: ";
	int bkt; 
	struct hashEntry *current_hash_entry;

	sprintf(structureValues, "%s", name);

	list_for_each_entry(current_entry, &mylist, list)
	{
		sprintf(structureValues + strlen(structureValues),"%d, ", current_entry->val);
	}
	printk(KERN_INFO "%s\n", structureValues);
	strcpy(linkedList, structureValues);
	strcpy(structureValues, "");
	strcpy(name, "Hash Table: ");
	sprintf(structureValues, "%s", name);
	hash_for_each(myHashTable, bkt, current_hash_entry, hash_list)
	{
		sprintf(structureValues + strlen(structureValues),"%d, ", current_hash_entry->val);
	}
	printk(KERN_INFO "%s\n", structureValues);
	strcpy(hashTable, structureValues);

}


static void destroy_linked_list_and_free(void)
{
	
	struct entry *current_entry, *next;
	struct hashEntry *current_hash_entry;
	int bkt;
	//delete linked list
	list_for_each_entry_safe(current_entry, next, &mylist, list) 
        {
 		list_del(&current_entry->list);
 		kfree(current_entry); 
	}
	//delete hash table
	hash_for_each(myHashTable, bkt, current_hash_entry, hash_list)
	{
		hash_del(&current_hash_entry->hash_list);
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

//Create proc entry
static int structures_proc_show(struct seq_file *m, void *v) {
  seq_printf(m, "%s\n%s\n", linkedList, hashTable);
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
//end proc entry
static int __init ex4_init(void)
{
	int err = 0;
	//create RB tree
	tree.run_node = RB_ROOT;
	if (!int_str) {
		printk(KERN_INFO "Missing \'int_str\' parameter, exiting\n");
		return -1;
	}
	
	
	err = parse_params();
	if (err)
		goto out;

	
	run_tests();
	proc_create("structures_proc", 0, NULL, &structures_proc_fops);
out:
	
	cleanup();
	return err;
}

static void __exit ex4_exit(void)
{
	remove_proc_entry("structures_proc", NULL);
	return;
}

//exit module
module_init(ex4_init);
module_exit(ex4_exit);

