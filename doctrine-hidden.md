# Doctrine's Hidden

We had a great question from a user recently and it had me thinking that this little used but super helpful 
Doctrine tool should get the spotlight!

The HIDDEN should allow you to select the field so that you can sort by it, but not include it in the results.

```
public function getCourses ForUserQueryBuilder (User $user)
{
    return $this->createQueryBuilder('course')
        ->leftJoin('course.orders', '0')
        ->andWhere('o.user = :user')
        ->setParameters('user', $user)
        // HIDDEN to select without adding it to what's returned
        ->addSelect('o.created as HIDDEN orderCreated' )
        ->addOrderBy( 'orderCreated', 'DESC')
    ;
}
```
