<?php

/**
 * Expose current user role as a context condition.
 */
class og_member_condition_context_condition_og_member extends context_condition {
  function condition_values() {
	$values = array();
	foreach (node_load_multiple(og_get_all_group()) as $nid => $node) {
		$values[$node->nid] = check_plain($node->title);
	}
	return $values;
  }

  function options_form($context){
	$defaults = $this->fetch_from_context($context, 'options');

	$options = array(
		'not_member' => t('Not a Group Member'),
		'member' => t('Group Member'),
		'admin' => t('Group Administrator')
	);

	return array(
		'member_status' => array(
			'#type' => 'radios',
			'#title' => t('Membership Status'),
			'#options' => $options,
			'#description' => t('Current users membership status'),
			'#default_value' => isset($defaults['member_status']) ? $defaults['member_status'] : $options['member']
		)
	);
  }

  function execute($user) {
  	foreach (node_load_multiple(og_get_all_group()) as $nid => $node) {
  		foreach ($this->get_contexts($node->nid) as $context) {
		  	$options = $this->fetch_from_context($context, 'options');
		  	$is_member = og_is_member($node->nid);
  			if (($options['member_status'] === 'member' && $is_member)
	  			|| ($options['member_status'] === 'not_member' && !$is_member)
		  		|| ($options['member_status'] === 'admin' 
		  			&& (in_array('administrator member', og_get_user_roles_name(og_get_user_roles($node->nid)))
		  				|| $user->uid === $node->uid))) {
  				$this->condition_met($context, $node->nid);
  			}
  		}
  	}
  }
}