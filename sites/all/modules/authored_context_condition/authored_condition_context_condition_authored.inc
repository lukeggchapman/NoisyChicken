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
        '#description' => t("Checking this box will make this condition fire if the user's has NOT authored a content of type"),
        '#default_value' => isset($defaults['negate_role']) ? $defaults['negate_role'] : 0,
      ),
    );
  }

  function execute($user) {
    $roles = $user->roles;
    foreach (node_type_get_types() as $type) {
      foreach ($this->get_contexts($type) as $context) {
        $options = $this->fetch_from_context($context, 'options');
        $nbr_authored = db_query('SELECT count(nid) as count FROM {node} WHERE type = :type AND uid = :uid', array(':uid' => $user->uid, ':type' => $type))->fetchField();
        if ($options['negate_role'] == 0 && $nbr_authored > 0) {
          $this->condition_met($context, $type);
        } elseif ($options['negate_role'] && $nbr_authored == 0) {
          $this->condition_met($context, $type);
        }
      }
    }
  }
}
