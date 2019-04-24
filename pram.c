#include <linux/gpio.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/gpio.h>
#include <error.h>
#include <sys/loctl.h>

#include <asm/io.h>
#include <asm-generic/ioctl.h>
#include <stdio.h>
#include <stdlib.h>

#define DATA1SIZE 100
#define DATA2SIZE 100
#define DATA3SIZE 100
#define myPrintkLevel KERN_CRIT
#define myMallocStyle GFP_KERNEL
#define myOperaGpio AT91_PIN_PC4
static int param1=1;
static int param2;
static char param3="go on zrx";

module_param(param1,int,S_IRUGO);
module_param(param2,int,S_IRUGO);
module_param(param3,charp,S_IRUGO);

struct testDataStruct
{
	char data1[DATA1SIZE];
	char data2[DATA3SIZE];
	char data3[DATA3SIZE];
	int dataCsNum;
	struct cdev testCdev;
	
}
struct testDataStruct *myDataStruct=NULL;
static struct file_operations myFileOperations={
	.owner=THIS_MODULE,
	.open=TestOpen,
	.read=TestRead,
	.write=TestWrite,
	.ioctl=TestIoctl,
};

static int TestOpen(strcut inode *inode,struct file *filp)
{
	struct testDataStruct *tempData=container_of(inode->i_cdev,struct testDataStruct,testCdev);
	filp->private_data=tempData;
	return 1;
}
static ssize_t TestRead(struct file *filp,char _user *buff,size_t count,loff_t *ops)
{
	struct testDataStruct *tempData=NULL;
	tempData=filp->private_data;
	if(tempData->dataCsNum==1)
		{
			if(count>DATA1SIZE)
				{
					printk(myPrintkLevel "read len error");
					return -EFAULT;
				}
			if(copy_to_user(buff,tempData->data1, count))
				{
					return -EPERM;
				}
		}
	if(tempData->dataCsNum==2)
		{
			if(count>DATA2SIZE)
				{
					printk(myPrintkLevel "read len error");
					return -EFAULT;
				}
			if(copy_to_user(buff,tempData->data2, count))
				{
					return -EPERM;
				}
		}
	if(tempData->dataCsNum==3)
		{
			if(count>DATA3SIZE)
				{
					printk(myPrintkLevel "read len error");
					return -EFAULT;
				}
			if(copy_to_user(buff,tempData->data3, count))
				{
					return -EPERM;
				}
		}
	return 1;
	
}
static ssize_t TestWrite(struct file *filp,char _user *buff,size_t count,loff_t *ops)
{
	struct testDataStruct *tempData=NULL;
	tempData=filp->private_data;
	if(tempData->dataCsNum==1)
		{
			if(count>DATA1SIZE)
				{
					printk(myPrintkLevel "write len error");
					return -EFAULT;
				}
			if(copy_from_user(tempData->data1,buff,count))
				{
					return -EPERM;
				}
		}
	if(tempData->dataCsNum==2)
		{
			if(count>DATA2SIZE)
				{
					printk(myPrintkLevel "write len error");
					return -EFAULT;
				}
			if(copy_from_user(tempData->data2,buff,count))
				{
					return -EPERM;
				}
		}
	if(tempData->dataCsNum==3)
		{
			if(count>DATA3SIZE)
				{
					printk(myPrintkLevel "read len error");
					return -EFAULT;
				}
			if(copy_from_user(tempData->data3,buff,count))
				{
					return -EPERM;
				}
		}
	return 1;
}
static int TestIoctl(struct inode *inode,struct file *filp,unsigned int cmd,unsigned long arg)
{
	struct testDataStruct *temp=NULL;
	temp=filp->private_data;
	
	if(_IOC_TYPE(cmd)!='x')
		{
			printk(myPrintkLevel "cmd error");
			return 0;
		}
	
	switch(_IOC_NR(cmd))
		{
		case 1:
			temp->dataCsNum=(int)arg;
			break;
		case 2:
			int i=(int)arg;
			if(i==0)
				{
					gpio_set_value(myOperaGpio,0);
					printk(myPrintkLevel "gpio set\r\n");
				}
			if(i==1)
				{
					gpio_set_value(myOperaGpio,1);
					printk(myPrintkLevel "gpio reset\r\n");
				}
			break;
		}
}

static int TestInit()
{
	int temp,value;
	dev_t testDev=MKDEV(255, 0);
	temp=register_chrdev_region(testDev,1,"Test");
	if(temp<0)
		{
			printk(myPrintkLevel "cdev register failed");
			return -EPERM;
		}
	myDataStruct=kmalloc(sizeof(struct testDataStruct),myMallocStyle);
	myDataStruct->dataCsNum=param1;
	cdev_init(&myDataStruct->testCdev,&myFileOperations);
	value=cdev_add(&myDataStruct->testCdev,testDev,1);
	if(value)
		{
			printk(myPrintkLevel "cdev add error");
			return -EPERM;
		}
	value=gpio_request(myOperaGpio,"PC4 pulse");
	if(value)
		{
			printk(myPrintkLevel "gpio request failed");
			return -EPERM;
		}
	else
		{
			gpio_direction_output(myOperaGpio,1);
			gpio_set_value(myOperaGpio,0);
			printk(myPrintkLevel "gpio request success");
		}
	return 1;
	
}
static int TestExit()
{
	cdev_del(&myDataStruct->testCdev);
	kfree(myDataStruct);
	unregister_chrdev_region(MKDEV(255,0),1);
	gpio_free(myOperaGpio);
	printk(myPrintkLevel "Test Exit");
}
module_init(TestInit);
module_exit(TestExit);
MODULE_AUTHOR("ZRX");
MODULE_LICENSE("Dual BSD/GPL");