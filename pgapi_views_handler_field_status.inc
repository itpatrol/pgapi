<?php

/**
* @file
* Definition of pgapi_views_handler_field_status.
*/

/**
* Render a field as a status value.
*
* @ingroup views_field_handlers
*/
class pgapi_views_handler_field_status extends views_handler_field {
  function render($values) {
    $value = $this->get_value($values);
    return '<div class="pgapi-status-' . $value . '">' . pgapi_get_status($value) . '</div>';
  }
}
