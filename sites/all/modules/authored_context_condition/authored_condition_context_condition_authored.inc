<?php

/**
 * Expose current user role as a context condition.
 */
class authored_condition_context_condition_authored extends context_condition {
  function condition_values() {
    $values = array();
    foreach (node_type_get_types() as $type) {
      $values[$type->type] = check_plain($type->name);
    }
    return $values;
  }

  function options_form($context){
    $defaults = $this->fetch_from_context($context, 'options');
    return array(
      'negate_role' => array(
        '#title' => t('Make authored a negative condition'),
        '#type' => 'checkbox',
        '#description' => t("Checking this box will make this condition fire if the user's has NOT authored all content of type"),
        '#default_value' => isset($defaults['negate_role']) ? $defaults['negate_role'] : 0,
      ),
    );
  }

  function execute($user) {
    $roles = $user->roles;
    foreach (node_type_get_types() as $type) {
      foreach ($this->get_contexts($type->type) as $context) {
        $options = $this->fetch_from_context($context, 'options');
        $result = db_query('SELECT DISTINCT type FROM {node} WHERE uid = :uid', array(':uid' => $user->uid));;
        $authored = array();
        foreach ($result as $row) {
          $authored[] = $row->type;
        }
        $type_values = $this->fetch_from_context($context, 'values');
        $met = true;
        foreach ($type_values as $type_key => $authored_type) {
          if ($options['negate_role'] == 0 && !in_array($authored_type, $authored)) {
            $met = false;
          } elseif ($options['negate_role'] && in_array($authored_type, $authored)) {
            $met = false;
          }
        }
        if ($met) {
          $this->condition_met($context, $type->type);
        }
      }
    }
  }
}
