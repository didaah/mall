<?php
// $Id$

/**
 * 商户、商品类别相关方法
 */

// public mall_category_form($category = NULL) {{{ 
/**
 * mall_category_form
 * 商户类别表单
 * 
 * @param object $category 
 *  一条商户类别记录，若有值，则编辑该记录
 * @access public
 * @return array 
 */
function mall_category_form($category = NULL) {
  $form = array(
    'fields' => array(),
    'settings' => array(
      '#validate' => array('mall_category_form_validate'), 
      '#ajax_validate' => 1,
    ),
  );

  $form['fields']['cid'] = array(
    '#type' => 'hidden',
    '#default_value' => !empty($category) ? $category->cid : 0,
    '#constant' => 1,
  );

  $form['fields']['name'] = array(
    '#title' => t('mall', '名称'),
    '#type' => 'textfield',
    '#weight' => -100,
    '#default_value' => !empty($category) ? $category->name : '',
    '#description' => t('mall', '不超过 64 个字符，不能重复'),
    '#required' => 1,
  );

  $form['fields']['type'] = array(
    '#title' => t('mall', '索引'),
    '#type' => 'textfield',
    '#weight' => -95,
    '#default_value' => !empty($category) ? $category->type : '',
    '#description' => t('mall', '只能是字母，不超过 32 个字符，不能重复'),
    '#required' => 1,
  );

  $options = array(0 => t('mall', '根'));

  if ($fetch = db_query('SELECT cid, pid, name FROM {mall_category} ORDER BY weight ASC, created DESC')) {
    $data = array();
    foreach ($fetch as $o) {
      $data[$o->pid][$o->cid] = $o;
    }
    mall_multi_level_options(0, $data, $options);
    if (!empty($category)) unset($options[$category->cid]);
  }

  // 类别的上级设定后不允许修改
  if (empty($category)) {
    $form['fields']['pid'] = array(
      '#title' => t('mall', '上级'),
      '#type' => 'select',
      '#weight' => -90,
      '#default_value' => !empty($category) ? $category->pid : 0,
      '#options' => $options,
      '#required' => 1,
    );
  }

  $form['fields']['status'] = array(
    '#title' => t('mall', '状态'),
    '#weight' => -80,
    '#type' => 'radio',
    '#default_value' => !empty($category) ? $category->status : 1,
    '#options' => array(t('mall', '隐藏'), t('mall', '启用')),
    '#required' => 1,
  );

  $form['fields']['weight'] = array(
    '#title' => t('mall', '排序'),
    '#type' => 'weight',
    '#weight' => -70,
    '#start' => -100,
    '#end' => 100,
    '#default_value' => !empty($category) ? $category->weight : 0,
    '#description' => t('mall', '数值越小越靠前'),
    '#required' => 1,
  );

  $form['fields']['data'] = array(
    '#weight' => -60,
  );

  $form['fields']['data']['description'] = array(
    '#title' => t('mall', '描述'),
    '#type' => 'textarea',
    '#default_value' => !empty($category) ? $category->data['description'] : '',
  );

  $form['fields']['button'] = array(
    '#type' => 'submit',
    '#weight' => 100,
    '#submit' => 'mall_category_form_submit'
  );

  module_alter_all('mall_category_form', $form, $category);

  return $form;
}
// }}}

function mall_category_form_validate(&$form, &$v) {
  return mall_category_validate($v['_dida_values'], $form);
}

function mall_category_form_submit(&$form, &$v) {
  if (mall_category_save($v['_dida_values'], $form)) {
    if (!empty($v['cid'])) $form['settings']['#redirect'] = url('admin/mall/category');
    dd_set_message(t('mall', '操作成功'));
  }
}

// public mall_category_save($v, &$form) {{{ 
/**
 * mall_category_save
 * 保存商户类别
 * 
 * @param object $v 
 *  一条商户类别记录
 * @param array $form 
 *  表单对象，通过 web 页面提交时，才有此值
 * @access public
 * @return object|bool
 */
function mall_category_save($v, &$form) {
  if (!empty($v->cid)) {
    $args = array('cid');
    // 不允许修改上级类别
    if (!empty($v->pid)) unset($v->pid);
  } else {
    $args = NULL;
    if (empty($v->created)) $v->created = time();
  }
  
  if (db_write_record('mall_category', $v, $args)) {
    module_invoke_all('mall_category_save', $v, $form);
    var_del('mall_category');
    return $v;
  }

  return false;
}
// }}}

// public mall_category_validate($v) {{{ 
/**
 * mall_category_validate
 * 商户类别验证
 * 
 * @param object $v 
 *  一条商户类别记录
 * @param array $form
 *  表单对象，通过 web 页面提交时，才有此值
 * @access public
 * @return void|array
 */
function mall_category_validate($v, &$form = NULL) {
  $e = NULL;

  if (empty($v->cid)) $v->cid = 0;

  if (mb_strlen($v->name) > 64) {
    $e['name']['#error'][] = t('mall', '不超过 64 个字符');
  } else if (db_query('SELECT cid FROM {mall_category} WHERE name = ? AND pid = ? AND cid != ?',
  array($v->name, $v->pid, $v->cid), array('return' => 'column'))) {
    $e['name']['#error'][] = t('mall', '!name 已经存在', array('!name' => $v->name));
  } else if (db_query('SELECT cid FROM {mall_category} WHERE type = ? AND pid = ? AND cid != ?',
  array($v->name, $v->pid, $v->cid), array('return' => 'column'))) {
    $e['type']['#error'][] = t('mall', '!name 已经存在', array('!name' => $v->type));
  } else {
    $e = module_invoke_all('mall_category_validate', $v, $form);
  }

  return $e;
}
// }}}

// public mall_category_delete($category) {{{ 
/**
 * mall_category_delete
 * 删除一条商户类别记录
 * 
 * @param object $category 
 *  一个商户类别对象
 * @param string $error
 *  若删除失败，错误信息
 * @access public
 * @return bool
 */
function mall_category_delete($category, &$error = '') {
  if (db_query('SELECT COUNT(*) FROM {mall_category} WHERE pid = ?',
  array($category->cid), array('return' => 'column'))) {
    $error = t('mall', '这是一个父类别，不能删除');
  } else if (db_query('SELECT COUNT(*) FROM {mall} WHERE cid = ?',
  array($category->cid), array('return' => 'column'))) {
    $error = t('mall', '该类别下商户不为空，不能删除');
  } else if (db_exec('DELETE FROM {mall_category} WHERE cid = ?', array($category->cid))) {
    // 删除类别的扩展字段
    field_group_delete('mall', 'fields_' . $category->cid);
    var_del('mall_category');
    return true;
  }
  return false;
}
// }}}

// public mall_item_category_form($category = NULL) {{{ 
/**
 * mall_item_category_form
 * 商品类别表单
 * 
 * @param object $category 
 *  一条商品类别记录，若有值，则编辑该记录
 * @access public
 * @return array 
 */
function mall_item_category_form($category = NULL) {
  $form = array(
    'fields' => array(),
    'settings' => array(
      '#validate' => array('mall_item_category_form_validate'), 
      '#ajax_validate' => 1,
    ),
  );

  $form['fields']['cid'] = array(
    '#type' => 'hidden',
    '#default_value' => !empty($category) ? $category->cid : 0,
    '#constant' => 1,
  );

  $form['fields']['name'] = array(
    '#title' => t('mall', '名称'),
    '#type' => 'textfield',
    '#weight' => -100,
    '#default_value' => !empty($category) ? $category->name : '',
    '#description' => t('mall', '不超过 64 个字符，不能重复'),
    '#required' => 1,
  );

  $form['fields']['type'] = array(
    '#title' => t('mall', '索引'),
    '#type' => 'textfield',
    '#weight' => -95,
    '#default_value' => !empty($category) ? $category->type : '',
    '#description' => t('mall', '只能是字母，不超过 32 个字符，不能重复'),
    '#required' => 1,
  );

  $options = array(0 => t('mall', '根'));

  if ($fetch = db_query('SELECT cid, pid, name FROM {item_category} ORDER BY weight ASC, created DESC')) {
    $data = array();
    foreach ($fetch as $o) {
      $data[$o->pid][$o->cid] = $o;
    }
    mall_multi_level_options(0, $data, $options);
    if (!empty($category)) unset($options[$category->cid]);
  }

  // 类别的上级设定后不允许修改
  if (empty($category)) {
    $form['fields']['pid'] = array(
      '#title' => t('mall', '上级'),
      '#type' => 'select',
      '#weight' => -90,
      '#default_value' => !empty($category) ? $category->pid : 0,
      '#options' => $options,
      '#required' => 1,
    );
  }

  $form['fields']['status'] = array(
    '#title' => t('mall', '状态'),
    '#weight' => -80,
    '#type' => 'radio',
    '#default_value' => !empty($category) ? $category->status : 1,
    '#options' => array(t('mall', '隐藏'), t('mall', '启用')),
    '#required' => 1,
  );

  $form['fields']['weight'] = array(
    '#title' => t('mall', '排序'),
    '#type' => 'weight',
    '#weight' => -70,
    '#start' => -100,
    '#end' => 100,
    '#default_value' => !empty($category) ? $category->weight : 0,
    '#description' => t('mall', '数值越小越靠前'),
    '#required' => 1,
  );

  $form['fields']['data'] = array(
    '#weight' => -60,
  );

  $form['fields']['data']['description'] = array(
    '#title' => t('mall', '描述'),
    '#type' => 'textarea',
    '#default_value' => !empty($category) ? $category->data['description'] : '',
  );

  $form['fields']['button'] = array(
    '#type' => 'submit',
    '#weight' => 100,
    '#submit' => 'mall_item_category_form_submit'
  );

  module_alter_all('mall_item_category_form', $form, $category);

  return $form;
}
// }}}

function mall_item_category_form_validate(&$form, &$v) {
  return mall_item_category_validate($v['_dida_values'], $form);
}

function mall_item_category_form_submit(&$form, &$v) {
  if (mall_item_category_save($v['_dida_values'], $form)) {
    if (!empty($v['cid'])) $form['settings']['#redirect'] = url('admin/mall/item/category');
    dd_set_message(t('mall', '操作成功'));
  }
}

// public mall_item_category_save($v, &$form) {{{ 
/**
 * mall_item_category_save
 * 保存商品类别
 * 
 * @param object $v 
 *  一条商品类别记录
 * @param array $form 
 *  表单对象，通过 web 页面提交时，才有此值
 * @access public
 * @return object|bool
 */
function mall_item_category_save($v, &$form) {
  if (!empty($v->cid)) {
    $args = array('cid');
    // 不允许修改上级类别
    if (!empty($v->pid)) unset($v->pid);
  } else {
    $args = NULL;
    if (empty($v->created)) $v->created = time();
  }
  
  if (db_write_record('item_category', $v, $args)) {
    module_invoke_all('mall_item_category_save', $v, $form);
    var_del('mall_item_category');
    return $v;
  }

  return false;
}
// }}}

// public mall_item_category_validate($v) {{{ 
/**
 * mall_item_category_validate
 * 商品类别验证
 * 
 * @param object $v 
 *  一条商品类别记录
 * @param array $form
 *  表单对象，通过 web 页面提交时，才有此值
 * @access public
 * @return void|array
 */
function mall_item_category_validate($v, &$form = NULL) {
  $e = NULL;

  if (empty($v->cid)) $v->cid = 0;

  if (mb_strlen($v->name) > 64) {
    $e['name']['#error'][] = t('mall', '不超过 64 个字符');
  } else if (db_query('SELECT cid FROM {item_category} WHERE name = ? AND pid = ? AND cid != ?',
  array($v->name, $v->pid, $v->cid), array('return' => 'column'))) {
    $e['name']['#error'][] = t('mall', '!name 已经存在', array('!name' => $v->name));
  } else if (db_query('SELECT cid FROM {item_category} WHERE type = ? AND pid = ? AND cid != ?',
  array($v->name, $v->pid, $v->cid), array('return' => 'column'))) {
    $e['type']['#error'][] = t('mall', '!name 已经存在', array('!name' => $v->type));
  } else {
    $e = module_invoke_all('mall_item_category_validate', $v, $form);
  }

  return $e;
}
// }}}

// public mall_item_category_delete($category) {{{ 
/**
 * mall_item_category_delete
 * 删除一条商品类别记录
 * 
 * @param object $category 
 *  一个商品类别对象
 * @param string $error
 *  若删除失败，错误信息
 * @access public
 * @return bool
 */
function mall_item_category_delete($category, &$error = '') {
  if (db_query('SELECT COUNT(*) FROM {item_category} WHERE pid = ?',
  array($category->cid), array('return' => 'column'))) {
    $error = t('mall', '这是一个父类别，不能删除');
  } else if (db_query('SELECT COUNT(*) FROM {mall_item} WHERE cid = ?',
  array($category->cid), array('return' => 'column'))) {
    $error = t('mall', '该类别下商户不为空，不能删除');
  } else if (db_exec('DELETE FROM {item_category} WHERE cid = ?', array($category->cid))) {
    // 删除类别的扩展字段
    field_group_delete('mall', 'item_fields_' . $category->cid);
    var_del('mall_item_category');
    return true;
  }
  return false;
}
// }}}
