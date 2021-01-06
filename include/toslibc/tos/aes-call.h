// SPDX-License-Identifier: LGPL-2.1
/*
 * Copyright (C) 2020 Fredrik Noring
 */

#ifndef _TOSLIBC_TOS_AES_CALL_H
#define _TOSLIBC_TOS_AES_CALL_H

#define __AES_CALL(call)						\
call( 10, appl_init__, (), (), (), ())					\
call( 11, appl_read,							\
		(ap_id, length),					\
		(),							\
		(message),						\
		(),,							\
		int16_t ap_id, int16_t length, void *message)		\
call( 12, appl_write,							\
		(ap_id, length),					\
		(),							\
		(message),						\
		(),,							\
		int16_t ap_id, int16_t length, const void *message)	\
call( 13, appl_find,							\
		(),							\
		(),							\
		(fname),						\
		(),,							\
		const char *fname)					\
call( 19, appl_exit, (), (), (), ())					\
									\
call( 20, evnt_keybd, (), (), (), ())					\
call( 21, evnt_button,							\
		(clicks, mask, state),					\
		(mx, my, button, kstate),				\
		(),							\
		(),,							\
		int16_t clicks, int16_t mask, int16_t state,		\
		int16_t *mx, int16_t *my,				\
		int16_t *button, int16_t *kstate)			\
call( 22, evnt_mouse,							\
		(flag, x, y, w, h),					\
		(mx, my, button, kstate),				\
		(),							\
		(),,							\
		int16_t flag,						\
		int16_t x, int16_t y, int16_t w, int16_t h,		\
		int16_t *mx, int16_t *my,				\
		int16_t *button, int16_t *kstate)			\
call( 23, evnt_mesag,							\
		(),							\
		(),							\
		(mesag),						\
		(),,							\
		struct aes_mesag *mesag)				\
call( 24, evnt_timer,							\
		(locount, hicount),					\
		(),							\
		(),							\
		(),,							\
		int16_t locount, int16_t hicount)			\
call( 25, evnt_multi,							\
		(events, bclicks, bmask, bstate, m1flag,		\
		 m1x, m1y, m1w, m1h, m2flag, m2x, m2y, m2w, m2h,	\
		 locount, hicount),					\
		(mx, my, mb, ks, kc, mc),				\
		(message),						\
		(),,							\
		int16_t events, int16_t bclicks, int16_t bmask,		\
		int16_t bstate, int16_t m1flag,				\
		int16_t m1x, int16_t m1y, int16_t m1w, int16_t m1h,	\
		int16_t m2flag, int16_t m2x, int16_t m2y,		\
		int16_t m2w, int16_t m2h, int16_t *message,		\
		int16_t locount, int16_t hicount,			\
		int16_t *mx, int16_t *my, int16_t *mb,			\
		int16_t *ks, int16_t *kc, int16_t *mc)			\
call( 26, evnt_dclick,							\
		(new_, flag), 						\
		(),							\
		(),							\
		(),,							\
		int16_t new_, int16_t flag)				\
									\
call( 30, menu_bar,							\
		(mode),							\
		(), 							\
		(tree),							\
		(),,							\
		struct aes_object *tree, int16_t mode)			\
call( 31, menu_icheck,							\
		(obj, check),						\
		(),							\
		(tree),							\
		(),,							\
		struct aes_object *tree, int16_t obj, int16_t check)	\
call( 32, menu_ienable,							\
		(obj, flag),						\
		(),							\
		(tree),							\
		(),,							\
		struct aes_object *tree, int16_t obj, int16_t flag)	\
call( 33, menu_tnormal,							\
		(obj, flag),						\
		(),							\
		(tree),							\
		(),,							\
		struct aes_object *tree, int16_t obj, int16_t flag)	\
call( 34, menu_text,							\
		(obj),							\
		(),							\
		(tree, text),						\
		(),,							\
		struct aes_object *tree, int16_t obj, const char *text)	\
call( 35, menu_register,						\
		(ap_id),						\
		(),							\
		(title),						\
		(),,							\
		int16_t ap_id, const char *title)			\
									\
call( 42, objc_draw,							\
		(obj, depth, ox, oy, ow, oh),				\
		(),							\
		(tree),							\
		(),,							\
		struct aes_object *tree, int16_t obj, int16_t depth,	\
		int16_t ox, int16_t oy, int16_t ow, int16_t oh)		\
call( 43, objc_find,							\
		(obj, depth, ox, oy),					\
		(),							\
		(tree),							\
		(),,							\
		struct aes_object *tree, int16_t obj, int16_t depth,	\
		int16_t ox, int16_t oy)					\
call( 44, objc_offset,							\
		(obj),							\
		(ox, oy),						\
		(tree),							\
		(),,							\
		struct aes_object *tree, int16_t obj,			\
		int16_t *ox, int16_t *oy)				\
call( 45, objc_order,							\
		(obj, pos),						\
		(),							\
		(tree),							\
		(),,							\
		struct aes_object *tree, int16_t obj, int16_t pos)	\
call( 46, objc_edit,							\
		(obj, kc, *idx, mode),					\
		(idx),							\
		(tree),							\
		(),,							\
		struct aes_object *tree, int16_t obj, int16_t kc,	\
		int16_t *idx, int16_t mode)				\
call( 47, objc_change,							\
		(obj, rsvd, ox, oy, ow, oh, newstate, drawflag),	\
		(),							\
		(tree),							\
		(),,							\
		struct aes_object *tree, int16_t obj, int16_t rsvd,	\
		int16_t ox, int16_t oy, int16_t ow, int16_t oh,		\
		int16_t newstate, int16_t drawflag)			\
									\
call( 50, form_do,							\
		(editobj),						\
		(),							\
		(tree),							\
		(),,							\
		struct aes_object *tree, int16_t editobj)		\
call( 51, form_dial,							\
		(mode, x1, y1, w1, h1, x2, y2, w2, h2),			\
		(),							\
		(),							\
		(),,							\
		int16_t mode, int16_t x1, int16_t y1,			\
		int16_t w1, int16_t h1, int16_t x2, int16_t y2,		\
		int16_t w2, int16_t h2)					\
call( 52, form_alert,							\
		(exit_button),						\
		(),							\
		(format),						\
		(),,							\
		int16_t exit_button, const char *format)		\
call( 53, form_error,							\
		(error),						\
		(),							\
		(),							\
		(),,							\
		int16_t error)						\
call( 54, form_center,							\
		(),							\
		(x, y, w, h),						\
		(tree),							\
		(),,							\
		struct aes_object *tree,				\
		int16_t *x, int16_t *y, int16_t *w, int16_t *h)		\
									\
call( 70, graf_rubberbox,						\
		(bx, by, minw, minh),					\
		(endw, endh),						\
		(),							\
		(),,							\
		int16_t bx, int16_t by,					\
		int16_t minw, int16_t minh,				\
		int16_t *endw, int16_t *endh)				\
call( 71, graf_dragbox,							\
		(w, h, sx, sy, bx, by, bw, bh),				\
		(endx, endy),						\
		(),							\
		(),,							\
		int16_t w, int16_t h, int16_t sx, int16_t sy,		\
		int16_t bx, int16_t by, int16_t bw, int16_t bh,		\
		int16_t *endx, int16_t *endy)				\
call( 72, graf_movebox,							\
		(bw, bh, sx, sy, ex, ey),				\
		(),							\
		(),							\
		(),,							\
		int16_t bw, int16_t bh,					\
		int16_t sx, int16_t sy,					\
		int16_t ex, int16_t ey)					\
call( 73, graf_growbox,							\
		(x1, y1, w1, h1, x2, y2, w2, h2),			\
		(),							\
		(),							\
		(),,							\
		int16_t x1, int16_t y1, int16_t w1, int16_t h1,		\
		int16_t x2, int16_t y2, int16_t w2, int16_t h2)		\
call( 74, graf_shrinkbox,						\
		(x1, y1, w1, h1, x2, y2, w2, h2),			\
		(),							\
		(),							\
		(),,							\
		int16_t x1, int16_t y1, int16_t w1, int16_t h1,		\
		int16_t x2, int16_t y2, int16_t w2, int16_t h2)		\
call( 75, graf_watchbox,						\
		(obj, instate, outstate),				\
		(),							\
		(tree),							\
		(),,							\
		struct aes_object *tree, int16_t obj,			\
		int16_t instate, int16_t outstate)			\
call( 76, graf_slidebox,						\
		(parent, obj, orient),					\
		(),							\
		(tree),							\
		(),,							\
		struct aes_object *tree, int16_t parent,		\
		int16_t obj, int16_t orient)				\
call( 77, graf_handle__,						\
		(),							\
		(wcell, hcell, wbox, hbox),				\
		(),							\
		(),,							\
		int16_t *wcell, int16_t *hcell,				\
		int16_t *wbox, int16_t *hbox)				\
call( 78, graf_mouse,							\
		(mode),							\
		(),							\
		(form),							\
		(),,							\
		int16_t mode, void *form)				\
call( 79, graf_mkstate,							\
		(),							\
		(mx, my, mb, ks),					\
		(),							\
		(),,							\
		int16_t *mx, int16_t *my, int16_t *mb, int16_t *ks)	\
									\
call( 90, fsel_input,							\
		(),							\
		(button),						\
		(path, file),						\
		(),,							\
		const char *path, const char *file, int16_t *button)	\
									\
call(100, wind_create,							\
		(kind, x, y, w, h),					\
		(),							\
		(),							\
		(),,							\
		int16_t kind, int16_t x, int16_t y,			\
		int16_t w, int16_t h)					\
call(101, wind_open,							\
		(handle, x, y, w, h),					\
		(),							\
		(),							\
		(),,							\
		int16_t handle, int16_t x, int16_t y,			\
		int16_t w, int16_t h)					\
call(102, wind_close,							\
		(handle),						\
		(),							\
		(),							\
		(),,							\
		int16_t handle)						\
call(103, wind_delete,							\
		(handle),						\
		(),							\
		(),							\
		(),,							\
		int16_t handle)						\
call(104, wind_get,	/* FIXME: control */				\
		(handle, mode),						\
		(parm1, parm2, parm3, parm4),				\
		(),							\
		(),,							\
		int16_t handle, int16_t mode,				\
		int16_t *parm1, int16_t *parm2,				\
		int16_t *parm3, int16_t *parm4)				\
call(105, wind_set,							\
		(handle, mode, parm1, parm2, parm3, parm4),		\
		(),							\
		(),							\
		(),,							\
		int16_t handle, int16_t mode,				\
		int16_t parm1, int16_t parm2,				\
		int16_t parm3, int16_t parm4)				\
call(106, wind_find,							\
		(x, y),							\
		(),							\
		(),							\
		(),,							\
		int16_t x, int16_t y)					\
call(107, wind_update,							\
		(mode),							\
		(),							\
		(),							\
		(),,							\
		int16_t mode)						\
call(108, wind_calc,							\
		(request, kind, x1, y1, w1, h1),			\
		(x2, y2, w2, h2),					\
		(),							\
		(),,							\
		int16_t request, int16_t kind,				\
		int16_t  x1, int16_t  y1, int16_t  w1, int16_t  h1,	\
		int16_t *x2, int16_t *y2, int16_t *w2, int16_t *h2)	\
									\
call(110, rsrc_load,							\
		(),							\
		(),							\
		(fname),						\
		(),,							\
		const char *fname)					\
call(111, rsrc_free, (), (), (), ())					\
call(112, rsrc_gaddr,							\
		(type, index),						\
		(),							\
		(),							\
		(addr),,						\
		int16_t type, int16_t index, void **addr)		\
call(113, rsrc_saddr,							\
		(type, index),						\
		(),							\
		(addr),							\
		(),,							\
		int16_t type, int16_t index, void *addr)		\
									\
call(120, shel_read,							\
		(),							\
		(),							\
		(name, tail),						\
		(),,							\
		const char *name, const char *tail)			\
call(121, shel_write,							\
		(mode, wisgr, wiscr),					\
		(),							\
		(cmd, tail),						\
		(),,							\
		int16_t mode, int16_t wisgr, int16_t wiscr,		\
		const char *cmd, const char *tail)			\
call(124, shel_find,							\
		(),							\
		(),							\
		(buf),							\
		(),,							\
		const char *buf)					\
call(125, shel_envrn,							\
		(),							\
		(),							\
		(value, name),						\
		(),,							\
		char **value, const char *name)

#endif /* _TOSLIBC_TOS_AES_CALL_H */
