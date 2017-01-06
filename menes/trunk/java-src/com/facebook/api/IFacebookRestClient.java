/*
  +---------------------------------------------------------------------------+
  | Facebook Development Platform Java Client                                 |
  +---------------------------------------------------------------------------+
  | Copyright (c) 2007 Facebook, Inc.                                         |
  | All rights reserved.                                                      |
  |                                                                           |
  | Redistribution and use in source and binary forms, with or without        |
  | modification, are permitted provided that the following conditions        |
  | are met:                                                                  |
  |                                                                           |
  | 1. Redistributions of source code must retain the above copyright         |
  |    notice, this list of conditions and the following disclaimer.          |
  | 2. Redistributions in binary form must reproduce the above copyright      |
  |    notice, this list of conditions and the following disclaimer in the    |
  |    documentation and/or other materials provided with the distribution.   |
  |                                                                           |
  | THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR      |
  | IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES |
  | OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.   |
  | IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,          |
  | INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT  |
  | NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, |
  | DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY     |
  | THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT       |
  | (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF  |
  | THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.         |
  +---------------------------------------------------------------------------+
  | For help with this library, contact developers-help@facebook.com          |
  +---------------------------------------------------------------------------+
*/

package com.facebook.api;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;

import java.net.URL;

import java.util.ArrayList;
import java.util.Collection;
import java.util.EnumSet;
import java.util.Map;
import java.util.Set;

/**
 * Generic interface for a FacebookRestClient, parameterized by output format.
 * For continually updated documentation, please refer to the
 * <a href="http://wiki.developers.facebook.com/index.php/API">
 * Developer Wiki</a>.
 */
public interface IFacebookRestClient<T> {
  public static final String TARGET_API_VERSION = "1.0";
  public static final String ERROR_TAG         = "error_response";
  public static final String FB_SERVER         = "api.facebook.com/restserver.php";
  public static final String SERVER_ADDR       = "http://" + FB_SERVER;
  public static final String HTTPS_SERVER_ADDR = "https://" + FB_SERVER;

  public void setDebug(boolean isDebug);

  public boolean isDebug();

  public boolean isDesktop();

  public void setIsDesktop(boolean isDesktop);

  /**
   * Sets the FBML for a user's profile, including the content for both the profile box
   * and the profile actions.
   * @param userId - the user whose profile FBML to set
   * @param fbmlMarkup - refer to the FBML documentation for a description of the markup and its role in various contexts
   * @return a boolean indicating whether the FBML was successfully set
   */
  public boolean profile_setFBML(CharSequence fbmlMarkup, Integer userId)
    throws FacebookException, IOException;

  /**
   * Gets the FBML for a user's profile, including the content for both the profile box
   * and the profile actions.
   * @param userId - the user whose profile FBML to set
   * @return a T containing FBML markup
   */
  public T profile_getFBML(Integer userId)
    throws FacebookException, IOException;

  /**
   * Recaches the referenced url.
   * @param url string representing the URL to refresh
   * @return boolean indicating whether the refresh succeeded
   */
  public boolean fbml_refreshRefUrl(String url)
    throws FacebookException, IOException;

  /**
   * Recaches the referenced url.
   * @param url the URL to refresh
   * @return boolean indicating whether the refresh succeeded
   */
  public boolean fbml_refreshRefUrl(URL url)
    throws FacebookException, IOException;

  /**
   * Recaches the image with the specified imageUrl.
   * @param imageUrl String representing the image URL to refresh
   * @return boolean indicating whether the refresh succeeded
   */
  public boolean fbml_refreshImgSrc(String imageUrl)
    throws FacebookException, IOException;

  /**
   * Recaches the image with the specified imageUrl.
   * @param imageUrl the image URL to refresh
   * @return boolean indicating whether the refresh succeeded
   */
  public boolean fbml_refreshImgSrc(URL imageUrl)
    throws FacebookException, IOException;

  /**
   * Publishes a Mini-Feed story describing an action taken by a user, and
   * publishes aggregating News Feed stories to the friends of that user.
   * Stories are identified as being combinable if they have matching templates and substituted values.
   * @param actorId the user into whose mini-feed the story is being published.
   * @param titleTemplate markup (up to 60 chars, tags excluded) for the feed story's title
   *        section. Must include the token <code>{actor}</code>.
   * @return whether the action story was successfully published; false in case
   *         of a permission error
   * @see <a href="http://wiki.developers.facebook.com/index.php/Feed.publishTemplatizedAction">
   *      Developers Wiki: Feed.publishTemplatizedAction</a>
   */
  public boolean feed_publishTemplatizedAction(Integer actorId, CharSequence titleTemplate)
    throws FacebookException, IOException;

  /**
   * Publishes a Mini-Feed story describing an action taken by a user, and
   * publishes aggregating News Feed stories to the friends of that user.
   * Stories are identified as being combinable if they have matching templates and substituted values.
   * @param actorId the user into whose mini-feed the story is being published.
   * @param titleTemplate markup (up to 60 chars, tags excluded) for the feed story's title
   *        section. Must include the token <code>{actor}</code>.
   * @param titleData (optional) contains token-substitution mappings for tokens that appear in
   *        titleTemplate. Should not contain mappings for the <code>{actor}</code> or
   *        <code>{target}</code> tokens. Required if tokens other than <code>{actor}</code>
   *        or <code>{target}</code> appear in the titleTemplate.
   * @param bodyTemplate (optional) markup to be displayed in the feed story's body section.
   *        can include tokens, of the form <code>{token}</code>, to be substituted using
   *        bodyData.
   * @param bodyData (optional) contains token-substitution mappings for tokens that appear in
   *        bodyTemplate. Required if the bodyTemplate contains tokens other than <code>{actor}</code>
   *        and <code>{target}</code>.
   * @param bodyGeneral (optional) additional body markup that is not aggregated. If multiple instances
   *        of this templated story are combined together, the markup in the bodyGeneral of
   *        one of their stories may be displayed.
   * @param targetIds The user ids of friends of the actor, used for stories about a direct action between
   *        the actor and these targets of his/her action. Required if either the titleTemplate or bodyTemplate
   *        includes the token <code>{target}</code>.
   * @param images (optional) additional body markup that is not aggregated. If multiple instances
   *        of this templated story are combined together, the markup in the bodyGeneral of
   *        one of their stories may be displayed.
   * @return whether the action story was successfully published; false in case
   *         of a permission error
   * @see <a href="http://wiki.developers.facebook.com/index.php/Feed.publishTemplatizedAction">
   *      Developers Wiki: Feed.publishTemplatizedAction</a>
   */
  public boolean feed_publishTemplatizedAction(Integer actorId, CharSequence titleTemplate,
                                               Map<String,CharSequence> titleData,
                                               CharSequence bodyTemplate,
                                               Map<String,CharSequence> bodyData,
                                               CharSequence bodyGeneral,
                                               Collection<Integer> targetIds,
                                               Collection<FeedImage> images
                                              )
    throws FacebookException, IOException;

  /**
   * Publish the notification of an action taken by a user to newsfeed.
   * @param title the title of the feed story (up to 60 characters, excluding tags)
   * @param body (optional) the body of the feed story (up to 200 characters, excluding tags)
   * @param images (optional) up to four pairs of image URLs and (possibly null) link URLs
   * @return whether the story was successfully published; false in case of permission error
   * @see <a href="http://wiki.developers.facebook.com/index.php/Feed.publishActionOfUser">
   *      Developers Wiki: Feed.publishActionOfUser</a>
   */
  public boolean feed_publishActionOfUser(CharSequence title, CharSequence body,
                                          Collection<FeedImage> images)
    throws FacebookException, IOException;

  /**
   * Publish the notification of an action taken by a user to newsfeed.
   * @param title the title of the feed story (up to 60 characters, excluding tags)
   * @param body (optional) the body of the feed story (up to 200 characters, excluding tags)
   * @return whether the story was successfully published; false in case of permission error
   * @see <a href="http://wiki.developers.facebook.com/index.php/Feed.publishActionOfUser">
   *      Developers Wiki: Feed.publishActionOfUser</a>
   */
  public boolean feed_publishActionOfUser(CharSequence title, CharSequence body)
    throws FacebookException, IOException;

  /**
   * Publish a story to the logged-in user's newsfeed.
   * @param title the title of the feed story
   * @param body the body of the feed story
   * @param images (optional) up to four pairs of image URLs and (possibly null) link URLs
   * @param priority
   * @return whether the story was successfully published; false in case of permission error
   * @see <a href="http://wiki.developers.facebook.com/index.php/Feed.publishStoryToUser">
   *      Developers Wiki: Feed.publishStoryToUser</a>
   */
  public boolean feed_publishStoryToUser(CharSequence title, CharSequence body,
                                         Collection<FeedImage> images, Integer priority)
    throws FacebookException, IOException;

  /**
   * Publish a story to the logged-in user's newsfeed.
   * @param title the title of the feed story
   * @param body the body of the feed story
   * @return whether the story was successfully published; false in case of permission error
   * @see <a href="http://wiki.developers.facebook.com/index.php/Feed.publishStoryToUser">
   *      Developers Wiki: Feed.publishStoryToUser</a>
   */
  public boolean feed_publishStoryToUser(CharSequence title, CharSequence body)
    throws FacebookException, IOException;

  /**
   * Publish a story to the logged-in user's newsfeed.
   * @param title the title of the feed story
   * @param body the body of the feed story
   * @param priority
   * @return whether the story was successfully published; false in case of permission error
   * @see <a href="http://wiki.developers.facebook.com/index.php/Feed.publishStoryToUser">
   *      Developers Wiki: Feed.publishStoryToUser</a>
   */
  public boolean feed_publishStoryToUser(CharSequence title, CharSequence body, Integer priority)
    throws FacebookException, IOException;

  /**
   * Publish a story to the logged-in user's newsfeed.
   * @param title the title of the feed story
   * @param body the body of the feed story
   * @param images (optional) up to four pairs of image URLs and (possibly null) link URLs
   * @return whether the story was successfully published; false in case of permission error
   * @see <a href="http://wiki.developers.facebook.com/index.php/Feed.publishStoryToUser">
   *      Developers Wiki: Feed.publishStoryToUser</a>
   */
  public boolean feed_publishStoryToUser(CharSequence title, CharSequence body,
                                         Collection<FeedImage> images)
    throws FacebookException, IOException;

  /**
   * Returns all visible events according to the filters specified. This may be used to find all events of a user, or to query specific eids.
   * @param eventIds filter by these event ID's (optional)
   * @param userId filter by this user only (optional)
   * @param startTime UTC lower bound (optional)
   * @param endTime UTC upper bound (optional)
   * @return T of events
   */
  public T events_get(Integer userId, Collection<Long> eventIds, Long startTime, Long endTime)
    throws FacebookException, IOException;

  /**
   * Retrieves the membership list of an event
   * @param eventId event id
   * @return T consisting of four membership lists corresponding to RSVP status, with keys
   * 'attending', 'unsure', 'declined', and 'not_replied'
   */
  public T events_getMembers(Number eventId)
    throws FacebookException, IOException;

  /**
   * Retrieves whether two users are friends.
   * @param userId1
   * @param userId2
   * @return T
   * @see <a href="http://wiki.developers.facebook.com/index.php/Friends.areFriends">
   *      Developers Wiki: Friends.areFriends</a>
   */
  public T friends_areFriends(int userId1, int userId2)
    throws FacebookException, IOException;

  /**
   * Retrieves whether pairs of users are friends.
   * Returns whether the first user in <code>userIds1</code> is friends with the first user in
   * <code>userIds2</code>, the second user in <code>userIds1</code> is friends with the second user in
   * <code>userIds2</code>, etc.
   * @param userIds1
   * @param userIds2
   * @return T
   * @see <a href="http://wiki.developers.facebook.com/index.php/Friends.areFriends">
   *      Developers Wiki: Friends.areFriends</a>
   */
  public T friends_areFriends(Collection<Integer> userIds1, Collection<Integer> userIds2)
    throws FacebookException, IOException;

  /**
   * Retrieves the friends of the currently logged in user.
   * @return array of friends
   */
  public T friends_get()
    throws FacebookException, IOException;

  /**
   * Retrieves the friends of the currently logged in user, who are also users
   * of the calling application.
   * @return array of friends
   */
  public T friends_getAppUsers()
    throws FacebookException, IOException;

  /**
   * Retrieves the requested info fields for the requested set of users.
   * @param userIds a collection of user IDs for which to fetch info
   * @param fields a set of ProfileFields
   * @return a T consisting of a list of users, with each user element
   * containing the requested fields.
   */
  public T users_getInfo(Collection<Integer> userIds, EnumSet<ProfileField> fields)
    throws FacebookException, IOException;

  /**
   * Retrieves the requested info fields for the requested set of users.
   * @param userIds a collection of user IDs for which to fetch info
   * @param fields a set of strings describing the info fields desired, such as "last_name", "sex"
   * @return a T consisting of a list of users, with each user element
   * containing the requested fields.
   */
  public T users_getInfo(Collection<Integer> userIds, Set<CharSequence> fields)
    throws FacebookException, IOException;

  /**
   * Retrieves the user ID of the user logged in to this API session
   * @return the Facebook user ID of the logged-in user
   */
  public int users_getLoggedInUser()
    throws FacebookException, IOException;

  /**
   * Retrieves an indicator of whether the logged-in user has added the
   * application associated with the _apiKey.
   * @return boolean indicating whether the user has added the app
   * @see <a href="http://wiki.developers.facebook.com/index.php/Users.isAppAdded">
   *      Developers Wiki: Users.isAppAdded</a>
   */
  public boolean users_isAppAdded()
    throws FacebookException, IOException;

  /**
   * Retrieves whether the logged-in user has granted the specified permission to this application.
   * @param permission an extended permission (e.g. FacebookExtendedPerm.MARKETPLACE,
   *        "photo_upload")
   * @return boolean indicating whether the user has the permission
   * @see FacebookExtendedPerm
   * @see <a href="http://wiki.developers.facebook.com/index.php/Users.hasAppPermission">
   *      Developers Wiki: Users.hasAppPermission</a>
   */
  public boolean users_hasAppPermission(CharSequence permission)
    throws FacebookException, IOException;

  /**
   * Sets the logged-in user's Facebook status.
   * Requires the status_update extended permission.
   * @return whether the status was successfully set
   * @see #users_hasAppPermission
   * @see FacebookExtendedPerm#STATUS_UPDATE
   * @see <a href="http://wiki.developers.facebook.com/index.php/Users.setStatus">
   *      Developers Wiki: Users.setStatus</a>
   */
  public boolean users_setStatus(String status)
    throws FacebookException, IOException;

  /**
   * Clears the logged-in user's Facebook status.
   * Requires the status_update extended permission.
   * @return whether the status was successfully cleared
   * @see #users_hasAppPermission
   * @see FacebookExtendedPerm#STATUS_UPDATE
   * @see <a href="http://wiki.developers.facebook.com/index.php/Users.setStatus">
   *      Developers Wiki: Users.setStatus</a>
   */
  public boolean users_clearStatus()
    throws FacebookException, IOException;

  /**
   * Used to retrieve photo objects using the search parameters (one or more of the
   * parameters must be provided).
   *
   * @param subjId retrieve from photos associated with this user (optional).
   * @param albumId retrieve from photos from this album (optional)
   * @param photoIds retrieve from this list of photos (optional)
   *
   * @return an T of photo objects.
   */
  public T photos_get(Integer subjId, Long albumId, Collection<Long> photoIds)
    throws FacebookException, IOException;

  /**
   * Used to retrieve photo objects using the search parameters (one or more of the
   * parameters must be provided).
   *
   * @param albumId retrieve from photos from this album (optional)
   * @param photoIds retrieve from this list of photos (optional)
   * @return an T of photo objects.
   * @see #photos_get(Integer, Long, Collection)
   * @see <a href="http://wiki.developers.facebook.com/index.php/Photos.get">
   *      Developers Wiki: Photos.get</a>
   */
  public T photos_get(Long albumId, Collection<Long> photoIds)
    throws FacebookException, IOException;

  /**
   * Used to retrieve photo objects using the search parameters (one or more of the
   * parameters must be provided).
   *
   * @param subjId retrieve from photos associated with this user (optional).
   * @param photoIds retrieve from this list of photos (optional)
   * @return an T of photo objects.
   * @see #photos_get(Integer, Long, Collection)
   * @see <a href="http://wiki.developers.facebook.com/index.php/Photos.get">
   *      Developers Wiki: Photos.get</a>
   */
  public T photos_get(Integer subjId, Collection<Long> photoIds)
    throws FacebookException, IOException;

  /**
   * Used to retrieve photo objects using the search parameters (one or more of the
   * parameters must be provided).
   *
   * @param subjId retrieve from photos associated with this user (optional).
   * @param albumId retrieve from photos from this album (optional)
   * @return an T of photo objects.
   * @see #photos_get(Integer, Long, Collection)
   * @see <a href="http://wiki.developers.facebook.com/index.php/Photos.get">
   *      Developers Wiki: Photos.get</a>
   */
  public T photos_get(Integer subjId, Long albumId)
    throws FacebookException, IOException;

  /**
   * Used to retrieve photo objects using the search parameters (one or more of the
   * parameters must be provided).
   *
   * @param photoIds retrieve from this list of photos (optional)
   * @return an T of photo objects.
   * @see #photos_get(Integer, Long, Collection)
   * @see <a href="http://wiki.developers.facebook.com/index.php/Photos.get">
   *      Developers Wiki: Photos.get</a>
   */
  public T photos_get(Collection<Long> photoIds)
    throws FacebookException, IOException;

  /**
   * Used to retrieve photo objects using the search parameters (one or more of the
   * parameters must be provided).
   *
   * @param albumId retrieve from photos from this album (optional)
   * @return an T of photo objects.
   * @see #photos_get(Integer, Long, Collection)
   * @see <a href="http://wiki.developers.facebook.com/index.php/Photos.get">
   *      Developers Wiki: Photos.get</a>
   */
  public T photos_get(Long albumId)
    throws FacebookException, IOException;

  /**
   * Used to retrieve photo objects using the search parameters (one or more of the
   * parameters must be provided).
   *
   * @param subjId retrieve from photos associated with this user (optional).
   * @return an T of photo objects.
   * @see #photos_get(Integer, Long, Collection)
   * @see <a href="http://wiki.developers.facebook.com/index.php/Photos.get">
   *      Developers Wiki: Photos.get</a>
   */
  public T photos_get(Integer subjId)
    throws FacebookException, IOException;

  /**
   * Retrieves album metadata. Pass a user id and/or a list of album ids to specify the albums
   * to be retrieved (at least one must be provided)
   *
   * @param userId   (optional) the id of the albums' owner (optional)
   * @param albumIds (optional) the ids of albums whose metadata is to be retrieved
   * @return album objects
   * @see <a href="http://wiki.developers.facebook.com/index.php/Photos.getAlbums">
   *      Developers Wiki: Photos.getAlbums</a>
   */
  public T photos_getAlbums(Integer userId, Collection<Long> albumIds)
    throws FacebookException, IOException;

  /**
   * Retrieves album metadata for albums owned by a user.
   * @param userId   (optional) the id of the albums' owner (optional)
   * @return album objects
   * @see <a href="http://wiki.developers.facebook.com/index.php/Photos.getAlbums">
   *      Developers Wiki: Photos.getAlbums</a>
   */
  public T photos_getAlbums(Integer userId)
    throws FacebookException, IOException;

  /**
   * Retrieves album metadata for a list of album IDs.
   * @param albumIds the ids of albums whose metadata is to be retrieved
   * @return album objects
   * @see <a href="http://wiki.developers.facebook.com/index.php/Photos.getAlbums">
   *      Developers Wiki: Photos.getAlbums</a>
   */
  public T photos_getAlbums(Collection<Long> albumIds)
    throws FacebookException, IOException;

  /**
   * Retrieves the tags for the given set of photos.
   * @param photoIds The list of photos from which to extract photo tags.
   * @return the created album
   */
  public T photos_getTags(Collection<Long> photoIds)
    throws FacebookException, IOException;

  /**
   * Creates an album.
   * @param albumName The list of photos from which to extract photo tags.
   * @return the created album
   */
  public T photos_createAlbum(String albumName)
    throws FacebookException, IOException;

  /**
   * Creates an album.
   * @param name The album name.
   * @param location The album location (optional).
   * @param description The album description (optional).
   * @return an array of photo objects.
   */
  public T photos_createAlbum(String name, String description, String location)
    throws FacebookException, IOException;

  /**
   * Adds several tags to a photo.
   * @param photoId The photo id of the photo to be tagged.
   * @param tags A list of PhotoTags.
   * @return a list of booleans indicating whether the tag was successfully added.
   */
  public T photos_addTags(Long photoId, Collection<PhotoTag> tags)
    throws FacebookException, IOException;

  /**
   * Adds a tag to a photo.
   * @param photoId The photo id of the photo to be tagged.
   * @param xPct The horizontal position of the tag, as a percentage from 0 to 100, from the left of the photo.
   * @param yPct The vertical position of the tag, as a percentage from 0 to 100, from the top of the photo.
   * @param taggedUserId The list of photos from which to extract photo tags.
   * @return whether the tag was successfully added.
   */
  public boolean photos_addTag(Long photoId, Integer taggedUserId, Double xPct, Double yPct)
    throws FacebookException, IOException;

  /**
   * Adds a tag to a photo.
   * @param photoId The photo id of the photo to be tagged.
   * @param xPct The horizontal position of the tag, as a percentage from 0 to 100, from the left of the photo.
   * @param yPct The list of photos from which to extract photo tags.
   * @param tagText The text of the tag.
   * @return whether the tag was successfully added.
   */
  public boolean photos_addTag(Long photoId, CharSequence tagText, Double xPct, Double yPct)
    throws FacebookException, IOException;

  /**
   * Uploads a photo to Facebook.
   * @param photo an image file
   * @return a T with the standard Facebook photo information
   * @see <a href="http://wiki.developers.facebook.com/index.php/Photos.upload">
   * Developers wiki: Photos.upload</a>
   */
  public T photos_upload(File photo)
    throws FacebookException, IOException;

  /**
   * Uploads a photo to Facebook.
   * @param photo an image file
   * @param caption a description of the image contents
   * @return a T with the standard Facebook photo information
   * @see <a href="http://wiki.developers.facebook.com/index.php/Photos.upload">
   * Developers wiki: Photos.upload</a>
   */
  public T photos_upload(File photo, String caption)
    throws FacebookException, IOException;

  /**
   * Uploads a photo to Facebook.
   * @param photo an image file
   * @param albumId the album into which the photo should be uploaded
   * @return a T with the standard Facebook photo information
   * @see <a href="http://wiki.developers.facebook.com/index.php/Photos.upload">
   * Developers wiki: Photos.upload</a>
   */
  public T photos_upload(File photo, Long albumId)
    throws FacebookException, IOException;

  /**
   * Uploads a photo to Facebook.
   * @param photo an image file
   * @param caption a description of the image contents
   * @param albumId the album into which the photo should be uploaded
   * @return a T with the standard Facebook photo information
   * @see <a href="http://wiki.developers.facebook.com/index.php/Photos.upload">
   * Developers wiki: Photos.upload</a>
   */
  public T photos_upload(File photo, String caption, Long albumId)
    throws FacebookException, IOException;

  /**
   * Retrieves the groups associated with a user
   * @param userId Optional: User associated with groups.
   * A null parameter will default to the session user.
   * @param groupIds Optional: group ids to query.
   * A null parameter will get all groups for the user.
   * @return array of groups
   */
  public T groups_get(Integer userId, Collection<Long> groupIds)
    throws FacebookException, IOException;

  /**
   * Retrieves the membership list of a group
   * @param groupId the group id
   * @return a T containing four membership lists of
   * 'members', 'admins', 'officers', and 'not_replied'
   */
  public T groups_getMembers(Number groupId)
    throws FacebookException, IOException;

  /**
   * Retrieves the results of a Facebook Query Language query
   * @param query : the FQL query statement
   * @return varies depending on the FQL query
   */
  public T fql_query(CharSequence query)
    throws FacebookException, IOException;

  /**
   * Retrieves the outstanding notifications for the session user.
   * @return a T containing
   * notification count pairs for 'messages', 'pokes' and 'shares',
   * a uid list of 'friend_requests', a gid list of 'group_invites',
   * and an eid list of 'event_invites'
   */
  public T notifications_get()
    throws FacebookException, IOException;

  /**
   * Send a notification message to the specified users.
   * @param recipientIds the user ids to which the message is to be sent
   * @param notification the FBML to display on the notifications page
   * @param email the FBML to send to the specified users via email, or null
   *        if no email should be sent
   * @return a URL, possibly null, to which the user should be redirected to finalize
   * the sending of the email
   */
  public URL notifications_send(Collection<Integer> recipientIds, CharSequence notification,
                                CharSequence email)
    throws FacebookException, IOException;

  /**
   * Call this function and store the result, using it to generate the
   * appropriate login url and then to retrieve the session information.
   * @return an authentication token
   */
  public String auth_createToken()
    throws FacebookException, IOException;

  /**
   * Call this function to retrieve the session information after your user has
   * logged in.
   * @param authToken the token returned by auth_createToken or passed back to your callback_url.
   */
  public String auth_getSession(String authToken)
    throws FacebookException, IOException;

  /**
   * Call this function to get the user ID.
   *
   * @return The ID of the current session's user, or -1 if none.
   */
  public int auth_getUserId(String authToken)
    throws FacebookException, IOException;

  /**
   * Create a marketplace listing. The create_listing extended permission is required.
   * @param showOnProfile whether
   * @return the id of the created listing
   * @see #users_hasAppPermission
   * @see FacebookExtendedPerm#MARKETPLACE
   * @see <a href="http://wiki.developers.facebook.com/index.php/Marketplace.createListing">
   *      Developers Wiki: marketplace.createListing</a>
   */
  public Long marketplace_createListing(Boolean showOnProfile, MarketplaceListing attrs)
    throws FacebookException, IOException;

  /**
   * Modify a marketplace listing. The create_listing extended permission is required.
   * @return the id of the edited listing
   * @see <a href="http://wiki.developers.facebook.com/index.php/Marketplace.createListing">
   *      Developers Wiki: marketplace.createListing</a>
   */
  public Long marketplace_editListing(Long listingId, Boolean showOnProfile, MarketplaceListing attrs)
    throws FacebookException, IOException;

  /**
   * Remove a marketplace listing. The create_listing extended permission is required.
   * @param listingId the listing to be removed
   * @return boolean indicating whether the listing was removed
   * @see #users_hasAppPermission
   * @see FacebookExtendedPerm#MARKETPLACE
   * @see <a href="http://wiki.developers.facebook.com/index.php/Marketplace.removeListing">
   *      Developers Wiki: marketplace.removeListing</a>
   */
  public boolean marketplace_removeListing(Long listingId)
    throws FacebookException, IOException;

  /**
   * Remove a marketplace listing. The create_listing extended permission is required.
   * @param listingId the listing to be removed
   * @param status MARKETPLACE_STATUS_DEFAULT, MARKETPLACE_STATUS_SUCCESS, or MARKETPLACE_STATUS_NOT_SUCCESS
   * @return boolean indicating whether the listing was removed
   * @see #users_hasAppPermission
   * @see FacebookExtendedPerm#MARKETPLACE
   * @see <a href="http://wiki.developers.facebook.com/index.php/Marketplace.removeListing">
   *      Developers Wiki: marketplace.removeListing</a>
   */
  public boolean marketplace_removeListing(Long listingId, CharSequence status)
    throws FacebookException, IOException;

  /**
   * Get the categories available in marketplace.
   * @return a T listing the marketplace categories
   * @see <a href="http://wiki.developers.facebook.com/index.php/Marketplace.getCategories">
   *      Developers Wiki: marketplace.getCategories</a>
   */
  public T marketplace_getCategories()
    throws FacebookException, IOException;

  /**
   * Get the subcategories available for a category.
   * @param category a category, e.g. "HOUSING"
   * @return a T listing the marketplace sub-categories
   * @see <a href="http://wiki.developers.facebook.com/index.php/Marketplace.getSubCategories">
   *      Developers Wiki: marketplace.getSubCategories</a>
   */
  public T marketplace_getSubCategories(CharSequence category)
    throws FacebookException, IOException;

  /**
   * Fetch marketplace listings, filtered by listing IDs and/or the posting users' IDs.
   * @param listingIds listing identifiers (required if uids is null/empty)
   * @param userIds posting user identifiers (required if listingIds is null/empty)
   * @return a T of marketplace listings
   * @see <a href="http://wiki.developers.facebook.com/index.php/Marketplace.getListings">
   *      Developers Wiki: marketplace.getListings</a>
   */
  public T marketplace_getListings(Collection<Long> listingIds, Collection<Integer> userIds)
    throws FacebookException, IOException;

  /**
   * Search for marketplace listings, optionally by category, subcategory, and/or query string.
   * @param category the category of listings desired (optional except if subcategory is provided)
   * @param subCategory the subcategory of listings desired (optional)
   * @param query a query string (optional)
   * @return a T of marketplace listings
   * @see <a href="http://wiki.developers.facebook.com/index.php/Marketplace.search">
   *      Developers Wiki: marketplace.search</a>
   */
  public T marketplace_search(CharSequence category, CharSequence subCategory, CharSequence query)
    throws FacebookException, IOException;
}
