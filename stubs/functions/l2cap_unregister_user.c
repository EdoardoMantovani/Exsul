void l2cap_unregister_user(struct l2cap_conn *conn, struct l2cap_user *user){
	if (list_empty(&user->list)){
		goto out_unlock;
	}
	list_del_init(&user->list);
	user->remove(conn, user);
	out_unlock:
}
