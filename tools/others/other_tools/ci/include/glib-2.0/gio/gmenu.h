/*
 * Copyright Â© 2011 Canonical Ltd.
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * licence, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
 * USA.
 *
 * Author: Ryan Lortie <desrt@desrt.ca>
 */

#ifndef __G_MENU_H__
#define __G_MENU_H__

#include <gio/gmenumodel.h>

G_BEGIN_DECLS

#define G_TYPE_MENU          (g_menu_get_type ())
#define G_MENU(inst)         (G_TYPE_CHECK_INSTANCE_CAST ((inst), \
                              G_TYPE_MENU, GMenu))
#define G_IS_MENU(inst)      (G_TYPE_CHECK_INSTANCE_TYPE ((inst), \
                              G_TYPE_MENU))

#define G_TYPE_MENU_ITEM     (g_menu_item_get_type ())
#define G_MENU_ITEM(inst)    (G_TYPE_CHECK_INSTANCE_CAST ((inst), \
                              G_TYPE_MENU_ITEM, GMenuItem))
#define G_IS_MENU_ITEM(inst) (G_TYPE_CHECK_INSTANCE_TYPE ((inst), \
                              G_TYPE_MENU_ITEM))

typedef struct _GMenuItem GMenuItem;
typedef struct _GMenu     GMenu;

GLIB_AVAILABLE_IN_2_32
GType       g_menu_get_type                         (void) G_GNUC_CONST;
GLIB_AVAILABLE_IN_2_32
GMenu *     g_menu_new                              (void);

void        g_menu_freeze                           (GMenu       *menu);

void        g_menu_insert_item                      (GMenu       *menu,
                                                     gint         position,
                                                     GMenuItem   *item);
void        g_menu_prepend_item                     (GMenu       *menu,
                                                     GMenuItem   *item);
void        g_menu_append_item                      (GMenu       *menu,
                                                     GMenuItem   *item);
void        g_menu_remove                           (GMenu       *menu,
                                                     gint         position);

void        g_menu_insert                           (GMenu       *menu,
                                                     gint         position,
                                                     const gchar *label,
                                                     const gchar *detailed_action);
void        g_menu_prepend                          (GMenu       *menu,
                                                     const gchar *label,
                                                     const gchar *detailed_action);
void        g_menu_append                           (GMenu       *menu,
                                                     const gchar *label,
                                                     const gchar *detailed_action);

void        g_menu_insert_section                   (GMenu       *menu,
                                                     gint         position,
                                                     const gchar *label,
                                                     GMenuModel  *section);
void        g_menu_prepend_section                  (GMenu       *menu,
                                                     const gchar *label,
                                                     GMenuModel  *section);
void        g_menu_append_section                   (GMenu       *menu,
                                                     const gchar *label,
                                                     GMenuModel  *section);

void        g_menu_insert_submenu                   (GMenu       *menu,
                                                     gint        position,
                                                     const gchar *label,
                                                     GMenuModel  *submenu);
void        g_menu_prepend_submenu                  (GMenu       *menu,
                                                     const gchar *label,
                                                     GMenuModel  *submenu);
void        g_menu_append_submenu                   (GMenu       *menu,
                                                     const gchar *label,
                                                     GMenuModel  *submenu);


GType       g_menu_item_get_type                    (void) G_GNUC_CONST;
GMenuItem * g_menu_item_new                         (const gchar *label,
                                                     const gchar *detailed_action);

GLIB_AVAILABLE_IN_2_34
GMenuItem * g_menu_item_new_from_model              (GMenuModel  *model,
                                                     gint         item_index);

GMenuItem * g_menu_item_new_submenu                 (const gchar *label,
                                                     GMenuModel  *submenu);

GMenuItem * g_menu_item_new_section                 (const gchar *label,
                                                     GMenuModel  *section);

GLIB_AVAILABLE_IN_2_34
GVariant *  g_menu_item_get_attribute_value         (GMenuItem   *menu_item,
                                                     const gchar *attribute,
                                                     const GVariantType *expected_type);
GLIB_AVAILABLE_IN_2_34
gboolean    g_menu_item_get_attribute               (GMenuItem   *menu_item,
                                                     const gchar *attribute,
                                                     const gchar *format_string,
                                                     ...);
GLIB_AVAILABLE_IN_2_34
GMenuModel *g_menu_item_get_link                    (GMenuItem   *menu_item,
                                                     const gchar *link);

void        g_menu_item_set_attribute_value         (GMenuItem   *menu_item,
                                                     const gchar *attribute,
                                                     GVariant    *value);
void        g_menu_item_set_attribute               (GMenuItem   *menu_item,
                                                     const gchar *attribute,
                                                     const gchar *format_string,
                                                     ...);
void        g_menu_item_set_link                    (GMenuItem   *menu_item,
                                                     const gchar *link,
                                                     GMenuModel  *model);
void        g_menu_item_set_label                   (GMenuItem   *menu_item,
                                                     const gchar *label);
void        g_menu_item_set_submenu                 (GMenuItem   *menu_item,
                                                     GMenuModel  *submenu);
void        g_menu_item_set_section                 (GMenuItem   *menu_item,
                                                     GMenuModel  *section);
void        g_menu_item_set_action_and_target_value (GMenuItem   *menu_item,
                                                     const gchar *action,
                                                     GVariant    *target_value);
void        g_menu_item_set_action_and_target       (GMenuItem   *menu_item,
                                                     const gchar *action,
                                                     const gchar *format_string,
                                                     ...);
void        g_menu_item_set_detailed_action         (GMenuItem   *menu_item,
                                                     const gchar *detailed_action);

G_END_DECLS

#endif /* __G_MENU_H__ */
