<?php
// $Id$

// public mall_page($mall) {{{ 
/**
 * mall_page
 * 商户浏览页面
 * 
 * @param object $mall 
 *  一条商户记录
 * @access public
 * @return string
 */
function mall_page($mall) {
  $title = array($mall->title);

  if (!empty($mall->category)) $title[] = $mall->category->name;
  
  dd_set_title($title);

  return theme('mall', $mall);
}
// }}}

// public mall_item_page($item) {{{ 
/**
 * mall_item_page
 * 商品浏览页面
 * 
 * @param object $item 
 * @access public
 * @return string
 */
function mall_item_page($item) {
  return theme('mall_item', $item);
}
// }}}

/**
 * 跳转到当前用户的店铺首页，非商家管理员用户无权访问
 */
function mall_page_goto() {
  if ($mall = mall_is_shop_admin()) dd_goto('mall/' . $mall->mid);

  dd_get_access();
}
