<?php

/**
* @file
* Definition of pgapi_views_handler_field_gateway.
*/

/**
* Render a field as a gateway value.
*
* @ingroup views_field_handlers
*/
class pgapi_views_handler_field_gateway extends views_handler_field {
  function render($values) {
    $value = $this->get_value($values);
    return pgapi_get_gateway_name($value);
  }
}
