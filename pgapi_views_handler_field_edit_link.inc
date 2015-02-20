<?php

/**
* @file
* Definition of pgapi_views_handler_field_edit_link.
*/

/**
* Render a field as a link to edit transaction.
*
* @ingroup views_field_handlers
*/
class pgapi_views_handler_field_edit_link extends views_handler_field_url {
  function query() {
  }

  function option_definition() {
    $options = parent::option_definition();

    $options['link_title'] = array('default' => t('Edit'));

    return $options;
  }

  function options_form(&$form, &$form_state) {
    $form['link_title'] = array(
      '#title' => t('Link title'),
      '#type' => 'textfield',
      '#default_value' => $this->options['link_title'],
    );

    parent::options_form($form, $form_state);
  }

  function render($values) {
    if ($this->options['display_as_link']) {
      $link_title = t($this->options['link_title']);

      return l($link_title, 'admin/reports/showtransactions/edit/' . $values->txnid, array('query' => drupal_get_destination()));
    }

    return $values->txnid;
  }
}
