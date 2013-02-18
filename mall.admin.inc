<?php
// $Id$

/**
 * Mall 模块管理首页，显示概况数据等
 */
function mall_admin_page() {
  
  return $output;
}

function _mall_admin_tabs() {
  dd_set_tabs(array(
    l(t('mall', '商城概况'), 'admin/mall'),
    l(t('mall', '商户列表'), 'admin/mall/list'),
    l(t('mall', '添加商户'), 'admin/mall/add'),
  ));
}

// public mall_admin_form($mall = NULL) {{{ 
/**
 * mall_admin_form
 * 添加/编辑商户
 * 
 * @param object $mall 
 * @access public
 * @return string
 */
function mall_admin_form($mall = NULL) {
  module_include_file('mall.form.inc', 'mall');
  dd_add_css(dd_get_path('module', 'mall') . '/mall.admin.css');
  _mall_admin_tabs();

  $cid = 0;

  if (empty($mall)) {
    if (empty($_GET['cid'])) {
      if ($fetch = db_query('SELECT cid, pid, name FROM {mall_category} WHERE status = 1 ORDER BY weight ASC, created ASC')) {
        $options = $data = array();
        foreach ($fetch as $o) {
          $data[$o->pid][$o->cid] = $o;
        }
        mall_multi_level_merge_options(0, $data, $options);

        $items = array();
        foreach ($options as $cid => $name) {
          $items[] = l(t(NULL, $name), 'admin/mall/add', array('query' => 'cid=' . $cid)); 
        }
        $output = theme('item_list', $items, t('mall', '请选择商户类别'), 'ul', array('id' => 'mall_admin_form_cid_links'));
        return $output;
      } else {
        dd_set_help(t('mall', '当前没有商户类别，可以增加商户类别，便于组织商户信息'));
      }
    } else if ($datas = mall_get_category()) {
      if (!empty($datas[$_GET['cid']]) && !empty($datas[$_GET['cid']]->status)) $cid = $_GET['cid'];
    }
  } else {
    dd_set_tabs(array(l(t('mall', '编辑商户'), 'admin/mall/edit/' . $mall->mid)));
  }

  $output = dd_get_form('mall_form', $mall, $cid);

  return $output;
}
// }}}

// public mall_admin_list() {{{ 
/**
 * mall_admin_list
 * 商户列表
 * 
 * @access public
 * @return string
 */
function mall_admin_list() {
  _mall_admin_tabs();

  $header = array(
    array('data' => t('mall', '商户'), 'field' => 'm.title'), 
    array('data' => t('mall', '管理员'), 'field' => 'm.uid'),
    array('data' => t('mall', '创建时间'), 'field' => 'm.created', 'sort' => 'desc'), 
    t('mall', '操作')
  );

  if ($fetch = pager_query('SELECT m.mid, m.cid, m.title, m.status, m.updated, m.created, m.uid, u.name 
  FROM {mall} m INNER JOIN {users} u ON m.uid = u.uid' . table_sql($header))) {
    $table = array();
    foreach ($fetch as $o) {
      $table[] = array(
        l($o->title, 'mall/' . $o->mid),
        $o->uid ? $o->name : t('mall', '未设置'),
        format_date($o->created),
        dd_imp(array(
          l(t('mall', '编辑'), 'admin/mall/edit/' . $o->mid),
          l(t('mall', '删除'), 'admin/mall/delete/' . $o->mid, array('attributes' => array('class' => 'confirmajax'))),
        ))
      );
    }
    $output = theme('table', $header, $table, array('class' => 'center_table'));
    $output .= pager_view();
  }

  return $output;
}
// }}}

// public mall_admin_item() {{{ 
/**
 * mall_admin_item
 * 商品列表
 * 
 * @access public
 * @return string
 */
function mall_admin_item() {

}
// }}}
