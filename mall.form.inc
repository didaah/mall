<?php
// $Id$

// public mall_form($mall = NULL) {{{ 
/**
 * mall_form
 * 商户表单
 * 
 * @param object $mall 
 *  一条商户记录，赋值则编辑该商户
 * @param int $cid
 *  商户类别，添加商户时才有此值
 * @param string $redirect
 *  跳转地址
 * @access public
 * @return array
 */
function mall_form($mall = NULL, $cid = 0, $redirect = NULL) {
  $form = array(
    'fields' => array(),
    'settings' => array(
      '#validate' => array('mall_form_validate'),
      '#redirect' => $redirect,
      '#ajax_validate' => 1,
    ),
  );

  $cid = !empty($mall) ? $mall->cid : $cid;

  $form['fields']['cid'] = array(
    '#type' => 'hidden',
    '#default_value' => $cid,
    '#constant' => 1,
  );

  $form['fields']['nid'] = $form['fields']['mid'] = array(
    '#type' => 'hidden',
    '#default_value' => !empty($mall) ? $mall->mid : 0,
    '#constant' => 1,
  );

  if (!empty($cid)) {
    $parents = '';
    if ($data = array_reverse(mall_get_category_parents($cid, mall_get_category()))) {
      foreach ($data as $o) {
        $parents .= $o->name . '-->';
      }
      $parents = rtrim($parents, '-->');
    }
    $form['fields']['_cid'] = array(
      '#title' => t('mall', '类别'),
      '#type' => 'value',
      '#weight' => -101,
      '#default_value' => $parents
    );
  }

  $form['fields']['title'] = array(
    '#title' => t('mall', '名称'),
    '#type' => 'textfield',
    '#weight' => -100,
    '#required' => 1,
    '#default_value' => !empty($mall) ? $mall->title : ''
  );

  if (user_access('admin mall edit')) {
    $form['fields']['name'] = array(
      '#title' => t('mall', '帐户'),
      '#type' => 'textfield',
      '#weight' => -80,
      '#required' => 1,
      '#autocomplete' => array(
        'url' => url('ajax', array('query' => 'op=auto&module=user')),
      ),
      '#description' => t('mall', '关联商户管理员帐号，该用户帐号必须已存在。建议设定后，不要随意更改'),
      '#default_value' => !empty($mall) ? $mall->name : ''
    );
   
    // 只有拥有编辑商户权限的人才能修改以下信息 
    $form['fields']['status'] = array(
      '#title' => t('mall', '状态'),
      '#weight' => -80,
      '#type' => 'radio',
      '#default_value' => !empty($mall) ? $mall->status : 1,
      '#options' => array(t('mall', '隐藏'), t('mall', '正常')),
      '#required' => 1,
    );

    $form['fields']['weight'] = array(
      '#title' => t('mall', '排序'),
      '#type' => 'weight',
      '#weight' => -70,
      '#start' => -100,
      '#end' => 100,
      '#default_value' => !empty($mall) ? $mall->weight : 0,
      '#description' => t('mall', '数值越小越靠前'),
      '#required' => 1,
    );
  }

  $form['fields']['body'] = array(
    '#title' => t('mall', '描述'),
    '#type' => 'textarea',
    '#default_value' => !empty($mall) ? $mall->body : '',
  );

  $form['fields']['button'] = array(
    '#type' => 'submit',
    '#weight' => 100,
    '#submit' => 'mall_form_submit'
  );

  field_get_node_form('mall', 'fields', $mall, $form);
  
  if (!empty($cid)) field_get_node_form('mall', 'fields_' . $cid, $mall, $form);

  module_alter_all('mall_form', $form, $mall);

  return $form;
}
// }}}

function mall_form_validate(&$form, &$v) {
  return mall_validate($v['_dida_values'], $form);  
}

function mall_form_submit(&$form, &$v) {
  if (mall_save($v['_dida_values'], $form)) {
    dd_set_message(t('mall', '操作成功'));
    if (empty($form['settings']['#redirect'])) {
      $form['settings']['#redirect'] = url('admin/mall/list');
    }
  }
}

// public mall_validate($v, $form) {{{ 
/**
 * mall_validate
 * 商户验证
 * 
 * @param object $v 
 *  待验证的商户对象
 * @param array $form 
 *  表单对象，通过 web 提交时有此值
 * @access public
 * @return void
 */
function mall_validate($v, $form = NULL) {
  $e = NULL;

  $v->title = trim($v->title);

  if (empty($v->mid)) $v->mid = 0;

  $v->nid = $v->mid;

  if (mb_strlen($v->title) > 64) {
    $e['title']['#error'][] = t('mall', '不能超过 64 个字符');
  } else if (db_query('SELECT mid FROM {mall} WHERE title = ? AND mid != ?',
  array($v->title, $v->mid), array('return' => 'column'))) {
    $e['title']['#error'][] = t('mall', '该商户名称已经存在，请换一个试试');
  } else if (!empty($v->name)) {
    if (!$v->uid = db_query('SELECT uid FROM {users} WHERE name = ?',
    array($v->name), array('return' => 'column'))) {
      $e['name']['#error'][] = t('mall', '该用户帐号不存在，请先添加帐号或换一个试试');
    } else if (db_query('SELECT mid FROM {mall} WHERE uid = ? AND mid != ?',
     array($v->uid, $v->mid), array('return' => 'column'))) {
      $e['name']['#error'][] = t('mall', '该用户已是其它商户的管理员，请换一个');
    }
  }

  if (empty($e)) $e = field_get_node_validate('mall', 'fields', $v);
  if (empty($e) && !empty($v->cid)) $e = field_get_node_validate('mall', 'fields_' . $v->cid, $v);
  if (empty($e)) $e = module_invoke_all('mall_validate', $v, $form);

  return $e;
}
// }}}

// public mall_save($v, $form) {{{ 
/**
 * mall_save
 * 保存商户
 * 
 * @param object $v 
 *  待保存的商户对象
 * @param array $form 
 *  表单对象，通过 web 提交时有此值
 * @access public
 * @return object|bool
 */
function mall_save($v, $form) {
  if (!empty($v->mid)) {
    $v->updated = time();
    $args = array('mid');
  } else {
    $args = NULL;
    if (empty($v->created)) $v->created = $v->updated = time();
  }

  if (db_write_record('mall', $v, $args)) {
    $v->nid = $v->mid;
    field_get_node_save('mall', 'fields', $v);
    if (!empty($v->cid)) field_get_node_save('mall', 'fields_' . $v->cid, $v);
    module_invoke_all('mall_save', $v, $form);
    return $v;
  }
}
// }}}

// public mall_item_form($item = NULL, $cid = 0, $redirect = NULL) {{{ 
/**
 * mall_item_form
 * 
 * @param object $item 
 *  一条商品记录，新增商品时，必须传递 $item->mid，即商户 id
 * @param string $redirect 
 *  跳转地址
 * @access public
 * @return array
 */
function mall_item_form($item, $redirect = NULL) {
  
  // 必须指定商铺 mid 和商品 cid
  if (empty($item->mid) || empty($item->cid)) return false;

  $form = array(
    'fields' => array(),
    'settings' => array(
      '#validate' => array('mall_item_form_validate'),
      '#redirect' => $redirect,
      '#ajax_validate' => 1,
    ),
  );
  
  $form['fields']['cid'] = array(
    '#type' => 'hidden',
    '#default_value' => $item->cid,
    '#constant' => 1,
  );

  $form['fields']['mid'] = array(
    '#type' => 'hidden',
    '#default_value' => $item->mid,
    '#constant' => 1,
  );

  $form['fields']['nid'] = $form['fields']['iid'] = array(
    '#type' => 'hidden',
    '#default_value' => !empty($item->iid) ? $item->iid : 0,
    '#constant' => 1,
  );
  
  $parents = '';
  if ($data = array_reverse(mall_get_category_parents($item->cid, mall_get_item_category()))) {
    foreach ($data as $o) {
      $parents .= $o->name . '-->';
    }
    $parents = rtrim($parents, '-->');
  }
  $form['fields']['_cid'] = array(
    '#title' => t('mall', '类别'),
    '#type' => 'value',
    '#weight' => -101,
    '#default_value' => $parents
  );

  $form['fields']['title'] = array(
    '#title' => t('mall', '名称'),
    '#type' => 'textfield',
    '#weight' => -100,
    '#required' => 1,
    '#default_value' => !empty($item->title) ? $item->title : ''
  );
    
  $form['fields']['uid'] = array(
    '#type' => 'hidden',
    '#default_value' => isset($item->uid) ? $item->uid : $GLOBALS['user']->uid,
    '#constant' => 1
  );
 
  $form['fields']['status'] = array(
    '#title' => t('mall', '状态'),
    '#weight' => -80,
    '#type' => 'radio',
    '#default_value' => isset($item->status) ? $item->status : 1,
    '#options' => array(t('mall', '隐藏'), t('mall', '正常')),
    '#required' => 1,
  );

  $form['fields']['weight'] = array(
    '#title' => t('mall', '排序'),
    '#type' => 'weight',
    '#weight' => -70,
    '#start' => -100,
    '#end' => 100,
    '#default_value' => isset($item->weight) ? $item->weight : 0,
    '#description' => t('mall', '数值越小越靠前'),
    '#required' => 1,
  );

  $form['fields']['body'] = array(
    '#title' => t('mall', '描述'),
    '#type' => 'textarea',
    '#default_value' => $item->body,
  );

  $form['fields']['button'] = array(
    '#type' => 'submit',
    '#weight' => 100,
    '#submit' => 'mall_item_form_submit'
  );

  field_get_node_form('mall', 'item_fields', $item, $form);
  
  field_get_node_form('mall', 'item_fields_' . $item->cid, $item, $form);

  module_alter_all('mall_form', $form, $item);

  return $form;
}
// }}}

function mall_item_form_validate(&$form, &$v) {
  return mall_item_validate($v['_dida_values'], $form);  
}

function mall_item_form_submit(&$form, &$v) {
  if (mall_item_save($v['_dida_values'], $form)) {
    dd_set_message(t('mall', '操作成功'));
  }
}

// public mall_item_validate($v, $form) {{{ 
/**
 * mall_item_validate
 * 商品验证
 * 
 * @param object $v 
 *  待验证的商品对象
 * @param array $form 
 *  表单对象，通过 web 提交时有此值
 * @access public
 * @return void
 */
function mall_item_validate($v, $form = NULL) {
  $e = NULL;

  $v->title = trim($v->title);

  if (empty($v->iid)) $v->iid = 0;

  $v->nid = $v->iid;

  if (empty($v->mid)) {
    dd_set_message(t('mall', '非法的请求，缺少商铺参数'), 'error');
    $e['title']['#error'][] = t('mall', '非法的请求，缺少商铺参数');
  } else if (mb_strlen($v->title) > 256) {
    $e['title']['#error'][] = t('mall', '不能超过 256 个字符');
  }

  if (empty($e)) $e = field_get_node_validate('mall', 'item_fields', $v);
  if (empty($e) && !empty($v->cid)) $e = field_get_node_validate('mall', 'item_fields_' . $v->cid, $v);
  if (empty($e)) $e = module_invoke_all('mall_item_validate', $v, $form);

  return $e;
}
// }}}

// public mall_item_save($v, $form) {{{ 
/**
 * mall_item_save
 * 保存商品
 * 
 * @param object $v 
 *  待保存的商品对象
 * @param array $form 
 *  表单对象，通过 web 提交时有此值
 * @access public
 * @return object|bool
 */
function mall_item_save($v, $form) {
  if (!empty($v->iid)) {
    $v->updated = time();
    $args = array('iid');
  } else {
    $args = NULL;
    if (empty($v->created)) $v->created = $v->updated = time();
    if (empty($v->uid)) $v->uid = $GLOBALS['user']->uid;
  }

  if (db_write_record('mall_item', $v, $args)) {
    $v->nid = $v->iid;
    field_get_node_save('mall', 'item_fields', $v);
    field_get_node_save('mall', 'item_fields_' . $v->cid, $v);
    module_invoke_all('mall_item_save', $v, $form);
    return $v;
  }
}
// }}}


