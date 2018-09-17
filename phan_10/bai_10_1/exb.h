/*
 * author: Nguyen Tien Dat
 * date: 17 Sept 2018
 */
#include <linux/device.h>

// cau truc exb_bus_type mo ta exb bus (example bus)
extern struct bus_type exb_bus_type;

// cau truc exb_driver mo ta driver tren exb bus
struct exb_driver {
	char *desc;
	struct module *module;
	struct device_driver driver;
};

/*
 * neu biet con tro, tro toi thanh vien driver, ta co the biet duoc
 * con tro, tro toi cau truc exb_driver chua bien thanh vien driver do
 */
#define to_exb_driver(drv) container_of(drv, struct exb_driver, driver);

// cau truc exb_dev mo ta thiet bi nam tren exb bus
struct exb_dev {
	char *desc;
	struct exb_driver *driver;
	struct device dev;
};

/*
 * neu biet con tro, tro toi thanh vien dev, ta co the biet duoc
 * con tro, tro toi cau truc exb_dev chua bien thanh vien dev do
 */
#define to_exb_dev(p) container_of(p, struct exb_dev, dev);

// ham dang ky mot thiet bi exb voi exb_core
extern int register_exb_device(struct exb_dev *);

// ham huy dang ky thiet bi exb voi exb_core
extern void unregister_exb_device(struct exb_dev *);

// ham dang ky driver voi exb_core
extern int exb_register_driver(struct exb_driver *);

// ham huy dang ky driver voi exb_core
extern void exb_unregister_driver(struct exb_driver *);
