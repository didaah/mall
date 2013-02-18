<?php
// $Id$

/**
 * @file 商户默认模板文件
 * @param object $mall 商户对象
 */
?>

<div class="mall_view" id="mall_view_<?php echo $mall->mid?>">
  <h2 class="mall_title"><?php echo $mall->title; ?></h2>
  <div class="mall_body"><?php echo $mall->body; ?></div>
</div>
