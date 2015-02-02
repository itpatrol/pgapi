<?php

/**
* @file
* Definition of pgapi_views_handler_field_amount.
*/

/**
* Render a field as a amount value.
*
* @ingroup views_field_handlers
*/
class pgapi_views_handler_field_amount extends views_handler_field {
  function option_definition() {
    $options = parent::option_definition();

    $options['format_price'] = array(
      'default' => TRUE,
    );

    return $options;
  }

  function options_form(&$form, &$form_state) {
    $form['format_price'] = array(
      '#type' => 'checkbox',
      '#title' => t('Format price'),
      '#description' => t('If checked, special handling will be used for plurality.'),
      '#default_value' => $this->options['format_price'],
    );

    parent::options_form($form, $form_state);
  }

  function render($values) {
    $value = $this->get_value($values);

    if ($this->options['format_price']) {
      return pgapi_format_price($value);
    }

    return $value;
  }
}
