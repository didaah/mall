<?php
// $Id$

// public mall_admin_category() {{{ 
/**
 * mall_admin_category
 * 商户类别管理
 * 
 * @access public
 * @return void
 */
function mall_admin_category() {
  module_include_file('mall.category.inc', 'mall');

	dd_set_tabs(array(
		l(t('mall', '商户类别'), 'admin/mall/category'),
		l(t('mall', '添加类别'), 'admin/mall/category/add'),
		l(t('mall', '通用字段'), 'admin/mall/fields'),
	));

  $args = arg();

  if (!empty($args[4])) $category = mall_category_load($args[4]);

  switch ($args[3]) {
    case 'add':
      $output = dd_get_form('mall_category_form');
    break;
    case 'edit':
      dd_set_tabs(array(l(t('mall', '编辑类别'), 'admin/mall/category/edit/' . $category->cid)));
      $output = dd_get_form('mall_category_form', $category);
    break;
    case 'delete':
      if (mall_category_delete($category)) {
        echo 'tr';
      } else {
        echo t('mall', '删除失败');
      }
    exit;
    case 'fields':
      if (empty($category->cid)) return false;

      if (db_query('SELECT COUNT(*) FROM {mall_category} WHERE pid = ?',
      array($category->cid), array('return' => 'column'))) {
        dd_set_message(t('mall', '父级类别不能添加扩展字段'), 'error');
        dd_goto('admin/mall/category');
      }

      $path = 'admin/mall/category/fields/' . $category->cid;

      dd_set_tabs(array(
        l(t('mall', '!name 类别扩展字段', array('!name' => $category->name)), $path)
      ));

      $tabs = array(
        l(t('mall', '字段管理'), $path),
        l(t('mall', '添加字段'), $path . '/add'),
        l(t('mall', '分组管理'), $path . '/groups'),
        l(t('mall', '导出字段'), $path . '/export')
      );
      
      switch ($args[5]) {
        case 'add':
          $field->type = 'fields_' . $category->cid;
          $field->module = 'mall';
          $field->redirect = url($path);
          $output = dd_get_form('field_form', $field);
        break;
        case 'export':
          $output = field_export(0, 'mall', 'fields_' . $category->cid);
        break;
        case 'groups':
          $output = dd_get_form('field_groups', 'mall', 'fields_' . $category->cid);
        break;
        default:
          $output = field_lists('mall', 'fields_' . $category->cid);
      }
      
      dd_set_sub_tabs($tabs);
    break;
    default:
      dd_set_help(t('mall', '类别可以多级，也可以一级，若非必要，建议只设置一级'));
      $output = _mall_admin_category();
  }

  return $output;
}
// }}}

// public mall_admin_fields() {{{ 
/**
 * mall_admin_fields
 * 商户默认字段，该字段将应用于所有类别
 * 
 * @access public
 * @return string
 */
function mall_admin_fields() {
	dd_set_tabs(array(
		l(t('mall', '商户类别'), 'admin/mall/category'),
		l(t('mall', '添加类别'), 'admin/mall/category/add'),
		l(t('mall', '通用字段'), 'admin/mall/fields'),
  ));

  dd_set_sub_tabs(array(
    l(t('mall', '字段管理'), 'admin/mall/fields'),
    l(t('mall', '添加字段'), 'admin/mall/fields/add'),
    l(t('mall', '分组管理'), 'admin/mall/fields/groups'),
    l(t('mall', '导出字段'), 'admin/mall/fields/export')
  ));

  switch (arg(3)) {
    case 'add':
      $field->type = 'fields';
      $field->module = 'mall';
      $field->redirect = url('admin/mall/fields');
      $output = dd_get_form('field_form', $field);
    break;
    case 'export':
      $output = field_export(0, 'mall', 'fields');
    break;
    case 'groups':
      $output = dd_get_form('field_groups', 'mall', 'fields');
    break;
    default:
      dd_set_help(t('mall', '通用字段将作用于所有商户类别'));
      $output = field_lists('mall', 'fields');
  }

  return $output; 
}
// }}}

// public mall_admin_item_category() {{{ 
/**
 * mall_admin_item_category
 * 商品类别管理
 * 
 * @access public
 * @return void
 */
function mall_admin_item_category() {
  module_include_file('mall.category.inc', 'mall');

	dd_set_tabs(array(
		l(t('mall', '商品类别'), 'admin/mall/item/category'),
		l(t('mall', '添加类别'), 'admin/mall/item/category/add'),
		l(t('mall', '通用字段'), 'admin/mall/item/fields'),
	));

  $args = arg();

  if (!empty($args[5])) $category = mall_item_category_load($args[5]);

  switch ($args[4]) {
    case 'add':
      $output = dd_get_form('mall_item_category_form');
    break;
    case 'edit':
      dd_set_tabs(array(l(t('mall', '编辑类别'), 'admin/mall/item/category/edit/' . $category->cid)));
      $output = dd_get_form('mall_item_category_form', $category);
    break;
    case 'delete':
      if (mall_item_category_delete($category)) {
        echo 'tr';
      } else {
        echo t('mall', '删除失败');
      }
    exit;
    case 'fields':
      if (empty($category->cid)) return false;

      if (db_query('SELECT COUNT(*) FROM {item_category} WHERE pid = ?',
      array($category->cid), array('return' => 'column'))) {
        dd_set_message(t('mall', '父级类别不能添加扩展字段'), 'error');
        dd_goto('admin/mall/item/category');
      }

      $path = 'admin/mall/item/category/fields/' . $category->cid;

      dd_set_tabs(array(
        l(t('mall', '!name 类别扩展字段', array('!name' => $category->name)), $path)
      ));

      $tabs = array(
        l(t('mall', '字段管理'), $path),
        l(t('mall', '添加字段'), $path . '/add'),
        l(t('mall', '分组管理'), $path . '/groups'),
        l(t('mall', '导出字段'), $path . '/export')
      );
      
      switch ($args[6]) {
        case 'add':
          $field->type = 'item_fields_' . $category->cid;
          $field->module = 'mall';
          $field->redirect = url($path);
          $output = dd_get_form('field_form', $field);
        break;
        case 'export':
          $output = field_export(0, 'mall', 'item_fields_' . $category->cid);
        break;
        case 'groups':
          $output = dd_get_form('field_groups', 'mall', 'item_fields_' . $category->cid);
        break;
        default:
          $output = field_lists('mall', 'item_fields_' . $category->cid);
      }
      
      dd_set_sub_tabs($tabs);
    break;
    default:
      dd_set_help(t('mall', '类别可以多级，也可以一级，若非必要，建议只设置一级'));
      $output = _mall_admin_item_category();
  }

  return $output;
}
// }}}

// public mall_admin_item_fields() {{{ 
/**
 * mall_admin_item_fields
 * 商品默认字段，该字段将应用于所有类别
 * 
 * @access public
 * @return string
 */
function mall_admin_item_fields() {
	dd_set_tabs(array(
		l(t('mall', '商品类别'), 'admin/mall/item/category'),
		l(t('mall', '添加类别'), 'admin/mall/item/category/add'),
		l(t('mall', '通用字段'), 'admin/mall/item/fields'),
  ));

  dd_set_sub_tabs(array(
    l(t('mall', '字段管理'), 'admin/mall/item/fields'),
    l(t('mall', '添加字段'), 'admin/mall/item/fields/add'),
    l(t('mall', '分组管理'), 'admin/mall/item/fields/groups'),
    l(t('mall', '导出字段'), 'admin/mall/item/fields/export')
  ));

  switch (arg(4)) {
    case 'add':
      $field->type = 'item_fields';
      $field->module = 'mall';
      $field->redirect = url('admin/mall/item/fields');
      $output = dd_get_form('field_form', $field);
    break;
    case 'export':
      $output = field_export(0, 'mall', 'item_fields');
    break;
    case 'groups':
      $output = dd_get_form('field_groups', 'mall', 'item_fields');
    break;
    default:
      dd_set_help(t('mall', '通用字段将作用于所有商品类别'));
      $output = field_lists('mall', 'item_fields');
  }

  return $output; 
}
// }}}

// protected _mall_admin_category() {{{ 
/**
 * _mall_admin_category
 * 商户类别列表
 * 
 * @access protected
 * @return string
 */
function _mall_admin_category() {
  $options = array();

  if ($fetch = db_query('SELECT cid, pid, name FROM {mall_category} ORDER BY weight ASC, created DESC')) {
    $data = array();

    foreach ($fetch as $o) {
      $data[$o->pid][$o->cid] = $o;
    }

    mall_multi_level_options(0, $data, $options, '', $full = 1);

    $table = array();
    foreach ($options as $o) {
      $links = array('edit' => l(t('mall', '编辑'), 'admin/mall/category/edit/' . $o->cid));
      if (empty($data[$o->cid])) {
        $links['delete'] = l(t('mall', '删除'), 'admin/mall/category/delete/' . $o->cid, array('attributes' => array('class' => 'confirmajax')));
        $links['fields'] = l(t('mall', '属性字段'), 'admin/mall/category/fields/' . $o->cid);
      }
      /**
       * hook_mall_category_links() 可自定义操作链接
       */
      module_alter_all('mall_category_links', $o, $links);
      $table[] = array(
        $o->name,
        array('data' => dd_imp($links), 'class' => 'center')
      );
    }
    $output = theme('table', array(t('mall', '名称'), t('mall', '操作')), $table);
  } else {
    dd_set_message(t('mall', '请先添加商户类别'));
    dd_goto('admin/mall/category/add');
  }
 
  return $output;
}
// }}}

// protected _mall_admin_item_category() {{{ 
/**
 * _mall_admin_item_category
 * 商品类别列表
 * 
 * @access protected
 * @return string
 */
function _mall_admin_item_category() {
  $options = array();

  if ($fetch = db_query('SELECT cid, pid, name FROM {item_category} ORDER BY weight ASC, created DESC')) {
    $data = array();

    foreach ($fetch as $o) {
      $data[$o->pid][$o->cid] = $o;
    }

    mall_multi_level_options(0, $data, $options, '', $full = 1);

    $table = array();
    foreach ($options as $o) {
      $links = array('edit' => l(t('mall', '编辑'), 'admin/mall/item/category/edit/' . $o->cid));
      if (empty($data[$o->cid])) {
        $links['delete'] = l(t('mall', '删除'), 'admin/mall/item/category/delete/' . $o->cid, array('attributes' => array('class' => 'confirmajax')));
        $links['fields'] = l(t('mall', '属性字段'), 'admin/mall/item/category/fields/' . $o->cid);
      }
      /**
       * hook_mall_category_links() 可自定义操作链接
       */
      module_alter_all('mall_item_category_links', $o, $links);
      $table[] = array(
        $o->name,
        array('data' => dd_imp($links), 'class' => 'center')
      );
    }
    $output = theme('table', array(t('mall', '名称'), t('mall', '操作')), $table);
  } else {
    dd_set_message(t('mall', '请先添加商品类别'));
    dd_goto('admin/mall/item/category/add');
  }
 
  return $output;
}
// }}}

