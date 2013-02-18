<?php
// $Id$

/**
 * @file 商家管理中心
 */

/**
 * @Implement of hook_user_center() 
 */
function mall_user_center_info() {
  return array(
    'mall' => array(
      'is_group' => true, // 定义组信息
      'weight' => -1000,
      'access_callback' => 'mall_is_shop_admin', // 验证是否为店铺管理员
      'list_title' => t('mall', '我的店铺'), // 列表标题
    ),
    'mall_item' => array(
      'title' => t('mall', '我的商品'),
      'weight' => -900,
    ),
    'mall_item_add' => array(
      'title' => t('mall', '新增商品'),
      'weight' => -800,
    ),
    'mall_block' => array(
      'weight' => -700,
      'title' => t('mall', '内容设置'),
    ),
    'mall_info' => array(
      'title' => t('mall', '基本信息'),
      'weight' => 900,
    ),
    'mall_show' => array(
      'title' => l(t('mall', '我的店铺'), 'mall/goto'),
      'weight' => 1000,
      'path' => 'mall/goto'
    )
  );
}

/**
 * @Implement of hook_user_center_view() 
 */
function mall_user_center_view($center) {
  if (substr($center->type, 0, 5) == 'mall_' && $mall = mall_is_shop_admin($center->user->uid)) {
    $mall = mall_load($mall->mid);
    $func = '_mall_user_center_view_' . $center->type;
    if (function_exists($func)) {
      $func($mall, $center);
    }
  }
}

/**
 * 商户基本信息编辑
 */
function _mall_user_center_view_mall_info($mall, $center) {
  module_include_file('mall.form.inc', 'mall');
  dd_add_css(dd_get_path('module', 'mall') . '/mall.admin.css');
  $center->body = dd_get_form('mall_form', $mall, 0, url('user/center/mall_info'));
}

/**
 * 添加商品
 */
function _mall_user_center_view_mall_item_add($mall, $center) {
  module_include_file('mall.form.inc', 'mall');
  dd_add_css(dd_get_path('module', 'mall') . '/mall.admin.css');

  $item = new stdClass;
  $item->mid = $mall->mid;

  // 获取商品类别
  if (!empty($_GET['cid']) && mall_get_item_category($_GET['cid'])) {
    $item->cid = $_GET['cid'];
    $center->body = dd_get_form('mall_item_form', $item, url('user/center/mall_item'));
    return true;
  }

  if ($fetch = db_query('SELECT cid, pid, name FROM {item_category} WHERE status = 1 ORDER BY weight ASC, created ASC')) {
    $options = $data = array();
    foreach ($fetch as $o) {
      $data[$o->pid][$o->cid] = $o;
    }

    mall_multi_level_merge_options(0, $data, $options);

    $items = array();

    foreach ($options as $cid => $name) {
      $items[] = l(t(NULL, $name), 'user/center/mall_item_add', array('query' => 'cid=' . $cid)); 
    }

    $center->body = theme('item_list', $items, t('mall', '请选择商品类别'), 'ul', array('id' => 'mall_admin_form_cid_links'));
    return false;
  } else {
    $center->body = t('mall', '当前没有商品类别，无法新增商品');
  }
}
