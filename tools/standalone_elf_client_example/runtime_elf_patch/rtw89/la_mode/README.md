"la_mode" (logic analyzer mode ?) refers to a low level ADC dumping mode implemented in the rtl8852cu driver,
this seems a really interesting feature which permits to dump 256k IQ samples over different lanes,
it seems to be not implemented in rtw89, with this patch, it will become feasible to achieve this low level mode dump.

Edit of 7 August: la_mode is way more complex: it permits to dump raw output from the ADC, the IQK (IQ calibration path under
Realtek), the DFIR and so on, it must be threated in a more extended way for being really useful for low level experiments
